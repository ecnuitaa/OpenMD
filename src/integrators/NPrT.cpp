/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Acknowledgement of the program authors must be made in any
 *    publication of scientific results based in part on use of the
 *    program.  An acceptable form of acknowledgement is citation of
 *    the article in which the program was described (Matthew
 *    A. Meineke, Charles F. Vardeman II, Teng Lin, Christopher
 *    J. Fennell and J. Daniel Gezelter, "OOPSE: An Object-Oriented
 *    Parallel Simulation Engine for Molecular Dynamics,"
 *    J. Comput. Chem. 26, pp. 252-271 (2005))
 *
 * 2. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * This software is provided "AS IS," without a warranty of any
 * kind. All express or implied conditions, representations and
 * warranties, including any implied warranty of merchantability,
 * fitness for a particular purpose or non-infringement, are hereby
 * excluded.  The University of Notre Dame and its licensors shall not
 * be liable for any damages suffered by licensee as a result of
 * using, modifying or distributing the software or its
 * derivatives. In no event will the University of Notre Dame or its
 * licensors be liable for any lost revenue, profit or data, or for
 * direct, indirect, special, consequential, incidental or punitive
 * damages, however caused and regardless of the theory of liability,
 * arising out of the use of or inability to use software, even if the
 * University of Notre Dame has been advised of the possibility of
 * such damages.
 */
 
#include "brains/SimInfo.hpp"
#include "brains/Thermo.hpp"
#include "integrators/IntegratorCreator.hpp"
#include "integrators/NPrT.hpp"
#include "primitives/Molecule.hpp"
#include "utils/OOPSEConstant.hpp"
#include "utils/simError.h"

namespace oopse {
  NPrT::NPrT(SimInfo* info) : NPT(info) {
    Globals* simParams = info_->getSimParams();
    if (!simParams->haveSurfaceTension()) {
      sprintf(painCave.errMsg,
              "If you use the NPT integrator, you must set tauBarostat.\n");
      painCave.severity = OOPSE_ERROR;
      painCave.isFatal = 1;
      simError();
    } else {
      surfaceTension= simParams->getSurfaceTension();
    }

  }
  void NPrT::evolveEtaA() {
    Mat3x3d hmat = currentSnapshot_->getHmat();
    double hz = hmat(2, 2);
    double Axy = hmat(0,0) * hmat(1, 1);
    double sx = -hz * (press(0, 0) - targetPressure/OOPSEConstant::pressureConvert);
    double sy = -hz * (press(1, 1) - targetPressure/OOPSEConstant::pressureConvert);
    eta(0,0) -= Axy * (sx - surfaceTension) / (NkBT*tb2);
    eta(1,1) -= Axy * (sy - surfaceTension) / (NkBT*tb2);
    eta(2,2) += dt2 *  instaVol * (press(2, 2) - targetPressure/OOPSEConstant::pressureConvert) / (NkBT*tb2);
    oldEta = eta;  
  }

  void NPrT::evolveEtaB() {
    Mat3x3d hmat = currentSnapshot_->getHmat();
    double hz = hmat(2, 2);
    double Axy = hmat(0,0) * hmat(1, 1);
    prevEta = eta;
    double sx = -hz * (press(0, 0) - targetPressure/OOPSEConstant::pressureConvert);
    double sy = -hz * (press(1, 1) - targetPressure/OOPSEConstant::pressureConvert);
    eta(0,0) = oldEta(0, 0) - Axy * (sx -surfaceTension) / (NkBT*tb2);
    eta(1,1) = oldEta(1, 1) - Axy * (sy -surfaceTension) / (NkBT*tb2);
    eta(2,2) = oldEta(2, 2) + dt2 *  instaVol *
	    (press(2, 2) - targetPressure/OOPSEConstant::pressureConvert) / (NkBT*tb2);
  }

  void NPrT::calcVelScale(){

    for (int i = 0; i < 3; i++ ) {
      for (int j = 0; j < 3; j++ ) {
	vScale(i, j) = eta(i, j);

	if (i == j) {
	  vScale(i, j) += chi;
	}
      }
    }
  }

  void NPrT::getVelScaleA(Vector3d& sc, const Vector3d& vel){
    sc = vScale * vel;
  }

  void NPrT::getVelScaleB(Vector3d& sc, int index ) {
    sc = vScale * oldVel[index];
  }

  void NPrT::getPosScale(const Vector3d& pos, const Vector3d& COM, int index, Vector3d& sc) {

    /**@todo */
    Vector3d rj = (oldPos[index] + pos)/2.0 -COM;
    sc = eta * rj;
  }

  void NPrT::scaleSimBox(){
    Mat3x3d scaleMat;

    scaleMat(0, 0) = exp(dt*eta(0, 0));
    scaleMat(1, 1) = exp(dt*eta(1, 1));    
    scaleMat(2, 2) = exp(dt*eta(2, 2));
    Mat3x3d hmat = currentSnapshot_->getHmat();
    hmat = hmat *scaleMat;
    currentSnapshot_->setHmat(hmat);

  }

  bool NPrT::etaConverged() {
    int i;
    double diffEta, sumEta;

    sumEta = 0;
    for(i = 0; i < 3; i++) {
      sumEta += pow(prevEta(i, i) - eta(i, i), 2);
    }
    
    diffEta = sqrt( sumEta / 3.0 );

    return ( diffEta <= etaTolerance );
  }

  double NPrT::calcConservedQuantity(){

    chi= currentSnapshot_->getChi();
    integralOfChidt = currentSnapshot_->getIntegralOfChiDt();
    loadEta();
    
    // We need NkBT a lot, so just set it here: This is the RAW number
    // of integrableObjects, so no subtraction or addition of constraints or
    // orientational degrees of freedom:
    NkBT = info_->getNGlobalIntegrableObjects()*OOPSEConstant::kB *targetTemp;

    // fkBT is used because the thermostat operates on more degrees of freedom
    // than the barostat (when there are particles with orientational degrees
    // of freedom).  
    fkBT = info_->getNdf()*OOPSEConstant::kB *targetTemp;    
    

    double totalEnergy = thermo.getTotalE();

    double thermostat_kinetic = fkBT * tt2 * chi * chi /(2.0 * OOPSEConstant::energyConvert);

    double thermostat_potential = fkBT* integralOfChidt / OOPSEConstant::energyConvert;

    SquareMatrix<double, 3> tmp = eta.transpose() * eta;
    double trEta = tmp.trace();
    
    double barostat_kinetic = NkBT * tb2 * trEta /(2.0 * OOPSEConstant::energyConvert);

    double barostat_potential = (targetPressure * thermo.getVolume() / OOPSEConstant::pressureConvert) /OOPSEConstant::energyConvert;

    Mat3x3d hmat = currentSnapshot_->getHmat();
    double hz = hmat(2, 2);
    double area = hmat(0,0) * hmat(1, 1);

    double conservedQuantity = totalEnergy + thermostat_kinetic + thermostat_potential +
      barostat_kinetic + barostat_potential - surfaceTension * area;

    return conservedQuantity;

  }

  void NPrT::loadEta() {
    eta= currentSnapshot_->getEta();

    //if (!eta.isDiagonal()) {
    //    sprintf( painCave.errMsg,
    //             "NPrT error: the diagonal elements of eta matrix are not the same or etaMat is not a diagonal matrix");
    //    painCave.isFatal = 1;
    //    simError();
    //}
  }

  void NPrT::saveEta() {
    currentSnapshot_->setEta(eta);
  }

}

