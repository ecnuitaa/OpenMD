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
 
/**
 * @file PolynomialTorsionType.hpp
 * @author teng lin
 * @date  11/16/2004
 * @version 1.0
 */ 

#ifndef TYPES_POLYNOMIALTORSIONTYPE_HPP
#define TYPES_POLYNOMIALTORSIONTYPE_HPP

#include <fstream>
#include "math/Polynomial.hpp"
#include "types/TorsionType.hpp"

namespace OpenMD {
  
  /**
   * @class PolynomialTorsionType PolynomialTorsionType.hpp "types/PolynomialTorsionType.hpp"
   * @todo documentation
   */
  class PolynomialTorsionType : public TorsionType{
    
  public:
    PolynomialTorsionType() { /* polynomial_ = new DoublePolynomial(); */ }
    
    RealType getCoefficient(int power) {
      return polynomial_.getCoefficient(power);
    }
    
    void addCoefficient(int power, RealType coefficient) {
      polynomial_.addCoefficient(power, coefficient);
    }
    
    void setCoefficient(int power, RealType coefficient) {
      polynomial_.setCoefficient(power, coefficient);
    }

    void setPolynomial(DoublePolynomial p) {
      polynomial_ = p;
    }
    
    virtual void calcForce(RealType cosPhi, RealType& V, RealType& dVdCosPhi) {
      V = polynomial_.evaluate(cosPhi);
      dVdCosPhi = polynomial_.evaluateDerivative(cosPhi); 
    }

    //friend std::ostream& operator <<(std::ostream& os, PolynomialTorsionType ptt);
    //friend std::ostream& operator <<(std::ostream& os, const PolynomialTorsionType& ptt);
  private:
    DoublePolynomial polynomial_;
};
  
    /*std::ostream& operator<< (std::ostream& os, PolynomialTorsionType ptt) {
    DoublePolynomial::const_iterator i;

    i = ptt.polynomial_.begin();
    
    if (i == ptt.polynomial_.end()) {
      os << "This Polynomial contains nothing" << std::endl;
      return os;
    }

     os << "This Polynomial contains below terms:" << std::endl;    
    
     while(true){
     os << i->second << "*" << "(cosPhi)" << "^" << i->first;

      if (++i == ptt.polynomial_.end()) {
	//if we reach the end of the polynomial pair, write out a
	//newline and then escape the loop
	os << std::endl;
	break;
      } else {
	//otherwise, write out a "+"
	os << " + ";
      }
    }
    
     os << std::endl;
     return os;
     }*/
  

} //end namespace OpenMD
#endif //TYPES_POLYNOMIALTORSIONTYPE_HPP

