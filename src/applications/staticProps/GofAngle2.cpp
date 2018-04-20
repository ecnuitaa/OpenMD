/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
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

#include <algorithm>
#include <fstream>
#include <sstream>
#include "applications/staticProps/GofAngle2.hpp"
#include "primitives/Atom.hpp"
#include "types/MultipoleAdapter.hpp"
#include "utils/simError.h"
#include "utils/Revision.hpp"

namespace OpenMD {
  
  GofAngle2::GofAngle2(SimInfo* info, 
                       const std::string& sele1, 
		       const std::string& sele2, int nangleBins)
    : RadialDistrFunc(info, sele1, sele2, nangleBins), 
      doSele3_(false), seleMan3_(info), evaluator3_(info) {

    setAnalysisType("Radial Distribution Function");
    string prefixFileName = info->getPrefixFileName();
    setOutputName(prefixFileName + ".gto");
    
    deltaCosAngle_ = 2.0 / nBins_;

    std::stringstream params;
    params << " nAngleBins = " << nBins_
           << ", deltaCosAngle = " << deltaCosAngle_;
    const std::string paramString = params.str();
    setParameterString( paramString );    

    histogram_.resize(nBins_);
    avgGofr_.resize(nBins_);
    for (unsigned int i = 0 ; i < nBins_; ++i) {
      histogram_[i].resize(nBins_);
      avgGofr_[i].resize(nBins_);
    }   
  }
  
  GofAngle2::GofAngle2(SimInfo* info, 
                       const std::string& sele1, 
		       const std::string& sele2, 
                       const std::string& sele3, int nangleBins)
    : RadialDistrFunc(info, sele1, sele2, nangleBins),
      doSele3_(true), seleMan3_(info), evaluator3_(info), 
      selectionScript3_(sele3) {

    string prefixFileName = info->getPrefixFileName();
    setOutputName(prefixFileName + ".gto");
    
    deltaCosAngle_ = 2.0 / nBins_;
    
    histogram_.resize(nBins_);
    avgGofr_.resize(nBins_);
    for (unsigned int i = 0 ; i < nBins_; ++i) {
      histogram_[i].resize(nBins_);
      avgGofr_[i].resize(nBins_);
    }    
    evaluator3_.loadScriptString(sele3);      
    if (!evaluator3_.isDynamic()) {
      seleMan3_.setSelectionSet(evaluator3_.evaluate());
    }
  }

  void GofAngle2::processNonOverlapping( SelectionManager& sman1, 
                                         SelectionManager& sman2) {
    StuntDouble* sd1;
    StuntDouble* sd2;
    StuntDouble* sd3;
    int i;    
    int j;
    int k;
    
    // This is the same as a non-overlapping pairwise loop structure:
    // for (int i = 0;  i < ni ; ++i ) {
    //   for (int j = 0; j < nj; ++j) {} 
    // }

    if (doSele3_) {
      if  (evaluator3_.isDynamic()) {
        seleMan3_.setSelectionSet(evaluator3_.evaluate());
      }
      if (sman1.getSelectionCount() != seleMan3_.getSelectionCount() ) {
        RadialDistrFunc::processNonOverlapping( sman1, sman2 );
      }

      for (sd1 = sman1.beginSelected(i), sd3 = seleMan3_.beginSelected(k); 
           sd1 != NULL && sd3 != NULL; 
           sd1 = sman1.nextSelected(i), sd3 = seleMan3_.nextSelected(k)) {
        for (sd2 = sman2.beginSelected(j); sd2 != NULL; 
             sd2 = sman2.nextSelected(j)) {
          collectHistogram(sd1, sd2, sd3);
        }
      }
    } else {
      RadialDistrFunc::processNonOverlapping( sman1, sman2 );
    }
  }

  void GofAngle2::processOverlapping( SelectionManager& sman) {
    StuntDouble* sd1;
    StuntDouble* sd2;
    StuntDouble* sd3;
    int i;    
    int j;
    int k;

    // This is the same as a pairwise loop structure:
    // for (int i = 0;  i < n-1 ; ++i ) {
    //   for (int j = i + 1; j < n; ++j) {} 
    // }
    
    if (doSele3_) {
      if  (evaluator3_.isDynamic()) {
        seleMan3_.setSelectionSet(evaluator3_.evaluate());
      }
      if (sman.getSelectionCount() != seleMan3_.getSelectionCount() ) {
        RadialDistrFunc::processOverlapping( sman);
      }
      for (sd1 = sman.beginSelected(i), sd3 = seleMan3_.beginSelected(k); 
           sd1 != NULL && sd3 != NULL; 
           sd1 = sman.nextSelected(i), sd3 = seleMan3_.nextSelected(k)) {
        for (j  = i, sd2 = sman.nextSelected(j); sd2 != NULL; 
             sd2 = sman.nextSelected(j)) {
          collectHistogram(sd1, sd2, sd3);
        }            
      }
    } else {
      RadialDistrFunc::processOverlapping( sman);
    }    
  }


  void GofAngle2::preProcess() {

    for (unsigned int i = 0; i < avgGofr_.size(); ++i) {
      std::fill(avgGofr_[i].begin(), avgGofr_[i].end(), 0);
    }
  }

  void GofAngle2::initializeHistogram() {
    npairs_ = 0;
    for (unsigned int i = 0; i < histogram_.size(); ++i)
      std::fill(histogram_[i].begin(), histogram_[i].end(), 0);
  }


  void GofAngle2::postProcess() {

    //std::for_each(avgGofr_.begin(), avgGofr_.end(), std::plus<std::vector<int>>)

  }

  void GofAngle2::collectHistogram(StuntDouble* sd1, StuntDouble* sd2) {
    bool usePeriodicBoundaryConditions_ = info_->getSimParams()->getUsePeriodicBoundaryConditions();
    
    if (sd1 == sd2) {
      return;
    }

    Vector3d pos1 = sd1->getPos();
    Vector3d pos2 = sd2->getPos();
    Vector3d r12 = pos1 - pos2;
    if (usePeriodicBoundaryConditions_) 
      currentSnapshot_->wrapVector(r12);

    AtomType* atype1 = static_cast<Atom*>(sd1)->getAtomType();
    AtomType* atype2 = static_cast<Atom*>(sd2)->getAtomType();
    MultipoleAdapter ma1 = MultipoleAdapter(atype1);
    MultipoleAdapter ma2 = MultipoleAdapter(atype2);

    if (!sd1->isDirectional()) {
      sprintf(painCave.errMsg, 
              "GofAngle2: attempted to use a non-directional object: %s\n", 
              sd1->getType().c_str());
      painCave.isFatal = 1;
      simError();  
    }

    if (!sd2->isDirectional()) {
      sprintf(painCave.errMsg, 
              "GofAngle2: attempted to use a non-directional object: %s\n", 
              sd2->getType().c_str());
      painCave.isFatal = 1;
      simError();  
    }

    Vector3d dipole1, dipole2;
    if (ma1.isDipole())         
        dipole1 = sd1->getDipole();
    else
        dipole1 = sd1->getA().transpose() * V3Z;

    if (ma2.isDipole())         
        dipole2 = sd2->getDipole();
    else
        dipole2 = sd2->getA().transpose() * V3Z;
    
    r12.normalize();
    dipole1.normalize();    
    dipole2.normalize();    
    

    RealType cosAngle1 = dot(r12, dipole1);
    RealType cosAngle2 = dot(dipole1, dipole2);

    RealType halfBin = (nBins_ - 1) * 0.5;
    int angleBin1 = int(halfBin * (cosAngle1 + 1.0));
    int angleBin2 = int(halfBin * (cosAngle2 + 1.0));

    ++histogram_[angleBin1][angleBin2];    
    ++npairs_;
  }

  void GofAngle2::collectHistogram(StuntDouble* sd1, StuntDouble* sd2, 
                                   StuntDouble* sd3) {
    bool usePeriodicBoundaryConditions_ = info_->getSimParams()->getUsePeriodicBoundaryConditions();

    if (sd1 == sd2) {
      return;
    }

    Vector3d p1 = sd1->getPos();
    Vector3d p3 = sd3->getPos();

    Vector3d c = 0.5 * (p1 + p3);
    Vector3d r13 = p3 - p1;

    Vector3d r12 = sd2->getPos() - c;
  
    if (usePeriodicBoundaryConditions_) {
      currentSnapshot_->wrapVector(r12);
      currentSnapshot_->wrapVector(r13);
    }
    r12.normalize();
    r13.normalize();

    if (!sd2->isDirectional()) {
      sprintf(painCave.errMsg, 
              "GofAngle2: attempted to use a non-directional object: %s\n", 
              sd2->getType().c_str());
      painCave.isFatal = 1;
      simError();  
    }

    AtomType* atype2 = static_cast<Atom*>(sd2)->getAtomType();
    MultipoleAdapter ma2 = MultipoleAdapter(atype2);

    Vector3d dipole2;
    if (ma2.isDipole())         
        dipole2 = sd2->getDipole();
    else
        dipole2 = sd2->getA().transpose() * V3Z;
    
    dipole2.normalize();    

    RealType cosAngle1 = dot(r12, r13);
    RealType cosAngle2 = dot(r13, dipole2);

    RealType halfBin = (nBins_ - 1) * 0.5;
    int angleBin1 = int(halfBin * (cosAngle1 + 1.0));
    int angleBin2 = int(halfBin * (cosAngle2 + 1.0));

    ++histogram_[angleBin1][angleBin2];    
    ++npairs_;

  }

  void GofAngle2::writeOutput() {
    std::ofstream ofs(outputFilename_.c_str());
    if (ofs.is_open()) {
      Revision r;
      ofs << "# " << getAnalysisType() << "\n";
      ofs << "# OpenMD " << r.getFullRevision() << "\n";
      ofs << "# " << r.getBuildDate() << "\n";
      ofs << "# selection script1: \"" << selectionScript1_ ;
      ofs << "\"\tselection script2: \"" << selectionScript2_ << "\"";
      if (doSele3_) {
        ofs << "\tselection script3: \"" << selectionScript3_ << "\"\n";
      } else {
        ofs << "\n";
      }

      if (!paramString_.empty())
        ofs << "# parameters: " << paramString_ << "\n";

      for (unsigned int i = 0; i < avgGofr_.size(); ++i) {
	// RealType cosAngle1 = -1.0 + (i + 0.5)*deltaCosAngle_;
        
	for(unsigned int j = 0; j < avgGofr_[i].size(); ++j) {
	  // RealType cosAngle2 = -1.0 + (j + 0.5)*deltaCosAngle_;
	  ofs <<avgGofr_[i][j]/nProcessed_ << "\t";
	}
	ofs << "\n";
      }
        
    } else {

      sprintf(painCave.errMsg, "GofAngle2: unable to open %s\n", 
              outputFilename_.c_str());
      painCave.isFatal = 1;
      simError();  
    }

    ofs.close();
  }

}
