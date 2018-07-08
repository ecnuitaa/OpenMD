/*
 * Copyright (c) 2012 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
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
 *
 * SUPPORT OPEN SCIENCE!  If you use OpenMD or its source code in your
 * research, please cite the appropriate papers when you publish your
 * work.  Good starting points are:
 *                                                                      
 * [1]  Meineke, et al., J. Comp. Chem. 26, 252-271 (2005).             
 * [2]  Fennell & Gezelter, J. Chem. Phys. 124, 234104 (2006).          
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 234107 (2008).          
 * [4]  Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [5]  Vardeman, Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */

#include "flucq/FluctuatingChargeObjectiveFunction.hpp"
#ifdef IS_MPI
#include "mpi.h"
#endif

namespace OpenMD{
  
  FluctuatingChargeObjectiveFunction::FluctuatingChargeObjectiveFunction(SimInfo* info, ForceManager* forceMan, FluctuatingChargeConstraints* fqConstraints)
    : info_(info), forceMan_(forceMan), fqConstraints_(fqConstraints), 
      thermo(info) {       
  }
  
  RealType FluctuatingChargeObjectiveFunction::value(const DynamicVector<RealType>& x) {
    
    setCoor(x);
      
    forceMan_->calcForces();
    fqConstraints_->applyConstraints();

    RealType pot = thermo.getPotential();

    return pot;
  }
  
  void FluctuatingChargeObjectiveFunction::gradient(DynamicVector<RealType>& grad, const DynamicVector<RealType>& x) {

    setCoor(x);         
    forceMan_->calcForces(); 
    fqConstraints_->applyConstraints();

    getGrad(grad);
  }
  
  RealType FluctuatingChargeObjectiveFunction::valueAndGradient(DynamicVector<RealType>& grad,
                                                                const DynamicVector<RealType>& x) {

    setCoor(x);

    forceMan_->calcForces();
    fqConstraints_->applyConstraints();
    
    getGrad(grad); 

    RealType pot = thermo.getPotential();
    
    return pot;
  }
  
  void FluctuatingChargeObjectiveFunction::setCoor(const DynamicVector<RealType> &x) const {
    SimInfo::MoleculeIterator i;
    Molecule::FluctuatingChargeIterator  j;
    Molecule* mol;
    Atom* atom;

    info_->getSnapshotManager()->advance();

    // All processors should have same coordinates, so we just need to
    // load up only the ones that we are responsible for:
    
    int ii = 0;
    int index;

    for (mol = info_->beginMolecule(i); mol != NULL; 
         mol = info_->nextMolecule(i)) {

      for (atom = mol->beginFluctuatingCharge(j); atom != NULL;
           atom = mol->nextFluctuatingCharge(j)) {
#ifdef IS_MPI
        index = displacements_[myrank_] + ii;
#else
        index = ii;
#endif
        atom->setFlucQPos(x[index]);
        ii++;
      }
    }
  }
  
  void FluctuatingChargeObjectiveFunction::getGrad(DynamicVector<RealType> &grad) {
    
    SimInfo::MoleculeIterator i;
    Molecule::FluctuatingChargeIterator  j;
    Molecule* mol;
    Atom* atom;       
    grad.setZero();
    
    int ii = 0;
    int index;
    for (mol = info_->beginMolecule(i); mol != NULL; 
         mol = info_->nextMolecule(i)) {

      for (atom = mol->beginFluctuatingCharge(j); atom != NULL;
           atom = mol->nextFluctuatingCharge(j)) {
#ifdef IS_MPI
        index = displacements_[myrank_] + ii;
#else
        index = ii;
#endif               
        grad[index] = -atom->getFlucQFrc();
        ii++;
      }
    }

#ifdef IS_MPI
    MPI_Allreduce(MPI_IN_PLACE, &grad[0], nFlucQ_, MPI_REALTYPE, MPI_SUM,
                  MPI_COMM_WORLD);
#endif
    
  } 

  DynamicVector<RealType> FluctuatingChargeObjectiveFunction::setInitialCoords() {

#ifdef IS_MPI
    MPI_Comm_size( MPI_COMM_WORLD, &nproc_);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank_);
    flucqOnProc_.clear();
    displacements_.clear();
    
    flucqOnProc_.resize(nproc_, 0);
    displacements_.resize(nproc_, 0);
#endif
    
    SimInfo::MoleculeIterator i;
    Molecule::FluctuatingChargeIterator  j;
    Molecule* mol;
    Atom* atom;

    nFlucQ_ = 0;
   
    for (mol = info_->beginMolecule(i); mol != NULL; 
         mol = info_->nextMolecule(i)) {

      for (atom = mol->beginFluctuatingCharge(j); atom != NULL;
           atom = mol->nextFluctuatingCharge(j)) {
        
        nFlucQ_++;
      }
    }

#ifdef IS_MPI
    MPI_Allgather(&nFlucQ_, 1, MPI_INT, &flucqOnProc_[0],
                  1, MPI_INT, MPI_COMM_WORLD);

    nFlucQ_ = 0;        
    for (int iproc = 0; iproc < nproc_; iproc++){
      nFlucQ_ += flucqOnProc_[iproc];
    }
    displacements_[0] = 0;
    
    for (int iproc = 1; iproc < nproc_; iproc++){
      displacements_[iproc] = displacements_[iproc-1] + flucqOnProc_[iproc-1];
    }
#endif

    DynamicVector<RealType> initCoords(nFlucQ_);
    
    int ii = 0;
    int index;
    for (mol = info_->beginMolecule(i); mol != NULL; 
         mol = info_->nextMolecule(i)) {

      for (atom = mol->beginFluctuatingCharge(j); atom != NULL;
           atom = mol->nextFluctuatingCharge(j)) {
#ifdef IS_MPI
        index = displacements_[myrank_] + ii;
#else
        index = ii;
#endif               
        initCoords[index] = atom->getFlucQPos();
        ii++;
      }
    }

#ifdef IS_MPI
    MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, &initCoords[0],
                   &flucqOnProc_[0], &displacements_[0],
                   MPI_REALTYPE, MPI_COMM_WORLD);
#endif
    
    return initCoords;
  }
}
