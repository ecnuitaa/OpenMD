/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Neil Firth

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.

    Adapted from the TNT project
    http://math.nist.gov/tnt/download.html

    This software was developed at the National Institute of Standards
    and Technology (NIST) by employees of the Federal Government in the
    course of their official duties. Pursuant to title 17 Section 105
    of the United States Code this software is not subject to copyright
    protection and is in the public domain. NIST assumes no responsibility
    whatsoever for its use by other parties, and makes no guarantees,
    expressed or implied, about its quality, reliability, or any other
    characteristic.

    We would appreciate acknowledgement if the software is incorporated in
    redistributable libraries or applications.

*/

/*! \file svd.hpp
    \brief singular value decomposition
*/

#ifndef math_svd_hpp
#define math_svd_hpp

#include "math/RectMatrix.hpp"

namespace oopse {

    //! Singular value decomposition
    /*! Refer to Golub and Van Loan: Matrix computation,
        The Johns Hopkins University Press

        \test the correctness of the returned values is tested by
              checking their properties.
    */
    class SVD {
      public:
        // constructor
        SVD(const RectMatrix&);
        // results
        const RectMatrix& U() const;
        const RectMatrix& V() const;
        const Vector& singularValues() const;
        RectMatrix S() const;
        Real norm2();
        Real cond();
        Integer rank();
        // utilities
        Vector solveFor(const Vector&) const;
      private:
        RectMatrix U_, V_;
        Vector s_;
        int m_, n_;
        bool transpose_;
    };
}


#endif

