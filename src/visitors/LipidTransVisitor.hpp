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

#ifndef VISITORS_LIPIDTRANSVISITOR_HPP
#define VISITORS_LIPIDTRANSVISITOR_HPP

#include "visitors/BaseVisitor.hpp"
#include "primitives/StuntDouble.hpp"
#include "visitors/AtomData.hpp"
#include "primitives/Molecule.hpp"
#include "brains/SimInfo.hpp"
#include "selection/SelectionEvaluator.hpp"
#include "selection/SelectionManager.hpp"
namespace oopse {

class LipidTransVisitor : public BaseVisitor {
    public:
        LipidTransVisitor(SimInfo* info, const std::string& originSeleScript, const std::string& refSeleScript) 
            : BaseVisitor(), info_(info), originEvaluator_(info), originSeleMan_(info), refEvaluator_(info), refSeleMan_(info), refSd_(NULL) {

         
            visitorName = "LipidTransVisitor";
            
            originEvaluator_.loadScriptString(originSeleScript);            
            if (!originEvaluator_.isDynamic()) {  
                originSeleMan_.setSelectionSet(originEvaluator_.evaluate());
                if (originSeleMan_.getSelectionCount() == 1) {
                    int i;
                    originDatom_ = dynamic_cast<DirectionalAtom*>(originSeleMan_.beginSelected(i));
                    if (originDatom_ ==  NULL) {
                        //error
                    }
                } else {
                    //error
                }
            }

            refEvaluator_.loadScriptString(refSeleScript);
            if (!refEvaluator_.isDynamic()) {  
                refSeleMan_.setSelectionSet(refEvaluator_.evaluate());
                if (refSeleMan_.getSelectionCount() == 1) {
                    int i;
                    refSd_ = refSeleMan_.beginSelected(i);
                    
                } else {
                    //error
                }
            }
            
        }
        
        virtual void visit(Atom* atom) { internalVisit(atom); }
        virtual void visit(DirectionalAtom* datom) { internalVisit(datom);}
        virtual void visit(RigidBody* rb) { internalVisit(rb);}

        virtual const std::string toString();

        void update();
        
    protected:
        void internalVisit(StuntDouble* sd);
        SimInfo* info_;
        SelectionEvaluator originEvaluator_;
        SelectionManager originSeleMan_;
        DirectionalAtom* originDatom_;
        SelectionEvaluator refEvaluator_;
        SelectionManager refSeleMan_;
        StuntDouble* refSd_;
        RotMat3x3d rotMat_;
        Vector3d origin_;
};
}
#endif
