/*
 *  Params.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include "GL2C.h"
#include "GL2ACJ.h"
#include "Params.h"

// the crucial map taking B_0 to B_infinity (and taking
// B_infinity to B_1') is of the form
// g(z) = p + l / z
//

GL2C constructG(const Params<Complex>& params)
{
	Complex I(0, 1);
	const Complex& sl = params.loxodromicSqrt;
	return GL2C(I*params.parabolic*sl, I/sl, I*sl, 0);
}

GL2C constructT(const Params<Complex>& params, int x, int y)
{
	return GL2C(1, params.lattice*double(y)+double(x), 0, 1);
}

GL2ACJ constructG(const Params<AComplex1Jet>& params)
{
	AComplex1Jet I(Complex(0, 1));
	const AComplex1Jet& sl = params.loxodromicSqrt;
	return GL2ACJ(I*params.parabolic*sl, I/sl, I*sl, AComplex1Jet(0.));
}

GL2ACJ constructT(const Params<AComplex1Jet>& params, int x, int y)
{
	return GL2ACJ(AComplex1Jet(1.), params.lattice*AComplex1Jet(double(y))+double(x), AComplex1Jet(0.), AComplex1Jet(1.));
}
