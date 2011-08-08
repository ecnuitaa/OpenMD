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
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 24107 (2008).          
 * [4]  Vardeman & Gezelter, in progress (2009).                        
 */
 
#include "UseTheForce/EADM_FF.hpp"
#include "UseTheForce/DarkSide/eam_interface.h"
#include "UseTheForce/ForceFieldFactory.hpp"

#include "io/DirectionalAtomTypesSectionParser.hpp"
#include "io/BaseAtomTypesSectionParser.hpp"
#include "io/AtomTypesSectionParser.hpp"
#include "io/LennardJonesAtomTypesSectionParser.hpp"
#include "io/ChargeAtomTypesSectionParser.hpp"
#include "io/MultipoleAtomTypesSectionParser.hpp"
#include "io/MetalNonMetalInteractionsSectionParser.hpp"
#include "io/EAMAtomTypesSectionParser.hpp"
#include "io/OptionSectionParser.hpp"

#include "UseTheForce/ForceFieldCreator.hpp"
#include "utils/simError.h"
namespace OpenMD {
    
  EADM_FF::EADM_FF(){

    //set default force field filename
    setForceFieldFileName("EADM.frc");

    //the order of adding section parsers are important
    //OptionSectionParser must come first to set options for other parsers

    spMan_.push_back(new OptionSectionParser(forceFieldOptions_));
    spMan_.push_back(new BaseAtomTypesSectionParser());
    spMan_.push_back(new AtomTypesSectionParser());
    spMan_.push_back(new DirectionalAtomTypesSectionParser(forceFieldOptions_));
    spMan_.push_back(new ChargeAtomTypesSectionParser(forceFieldOptions_));
    spMan_.push_back(new MultipoleAtomTypesSectionParser(forceFieldOptions_)); 
    spMan_.push_back(new MetalNonMetalInteractionsSectionParser(forceFieldOptions_));
    spMan_.push_back(new EAMAtomTypesSectionParser(forceFieldOptions_));

  }

  void EADM_FF::parse(const std::string& filename) {
    ifstrstream* ffStream;
    ffStream = openForceFieldFile(filename);

    spMan_.parse(*ffStream, *this);

    ForceField::AtomTypeContainer::MapTypeIterator i;
    AtomType* at;
    ForceField::AtomTypeContainer::MapTypeIterator j;
    AtomType* at2;
    ForceField::NonBondedInteractionTypeContainer::MapTypeIterator k;
    NonBondedInteractionType* nbit;

    for (at = atomTypeCont_.beginType(i); at != NULL; at = atomTypeCont_.nextType(i)) {
      // useBase sets the responsibilities, and these have to be done 
      // after the atomTypes and Base types have all been scanned:

      std::vector<AtomType*> ayb = at->allYourBase();      
      if (ayb.size() > 1) {
        for (int j = ayb.size()-1; j > 0; j--) {
          
          ayb[j-1]->useBase(ayb[j]);

        }
      }
      at->makeFortranAtomType();
    }

    for( at = atomTypeCont_.beginType(i); at != NULL; at = atomTypeCont_.nextType(i)) {
      at->complete();
    }

    hasEAMtypes_ = false;
    for (at = atomTypeCont_.beginType(i); at != NULL;
         at = atomTypeCont_.nextType(i)) {
      if (at->isEAM())
        hasEAMtypes_ = true;
    }
  
    /* to handle metal-nonmetal interactions, first we loop over
       all atom types: */
   
    for (at = atomTypeCont_.beginType(i); at != NULL;
         at = atomTypeCont_.nextType(i)) {
    
      /* if we find a metallic atom, we need to compare against
         all other atom types */
    
      if (at->isEAM()) {
    
        /* loop over all other atom types */
        for (at2 = atomTypeCont_.beginType(j); at2 != NULL;
             at2 = atomTypeCont_.nextType(j)) {
    
          /* if the other partner is not a metallic type, we need to
             look for explicit non-bonded interactions */
          if (!at2->isEAM()) {

            /* get the name and ident of the metallic atom type */
            std::string at1s = at->getName();
            int atid1 = at->getIdent();

            /* get the name and ident of the nonmetallic atom type */
            std::string at2s = at2->getName();
            int atid2 = at2->getIdent();
	    
	    
            /* look for a match in the non-bonded interactions parsed
               from the force field file */
            nbit = getNonBondedInteractionType(at1s, at2s);
    
            /* if we found a match (even a partial match), punt to the
               interaction to poke our info down to fortran. */
            if (nbit != NULL){ 
	       nbit->tellFortran(atid1, atid2);
	    }
          }
        }
      }
    }

    delete ffStream;
  }


  RealType EADM_FF::getRcutFromAtomType(AtomType* at){
    RealType rcut = 0.0;    
    if (at->isEAM()) {
      GenericData* data = at->getPropertyByName("EAM");
      if (data != NULL) {
	EAMParamGenericData* eamData = dynamic_cast<EAMParamGenericData*>(data);

	if (eamData != NULL) {

	  EAMParam& eamParam = eamData->getData();
	  rcut =  eamParam.rcut;
	} else {
	  sprintf( painCave.errMsg,
		   "Can not cast GenericData to EAMParam\n");
	  painCave.severity = OPENMD_ERROR;
	  painCave.isFatal = 1;
	  simError();          
	}
      } else {
	sprintf( painCave.errMsg, "Can not find EAM Parameters\n");
	painCave.severity = OPENMD_ERROR;
	painCave.isFatal = 1;
	simError();          
      }
    }    else {
      rcut = ForceField::getRcutFromAtomType(at);
    }
   
    return rcut;    
  }

  EADM_FF::~EADM_FF(){
    //We need to clean up the fortran side so we don't have bad things happen if
    //we try to create a second EADM force field.
    destroyEAMTypes();
  }
} //end namespace OpenMD