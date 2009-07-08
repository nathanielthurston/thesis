/*
 *  Box.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#import "Box.h"
#include <math.h>

double scale[6];
static bool scaleInitialized = false; 
Box::Box() {
	if (!scaleInitialized) {
		scaleInitialized = true;
		for (int i = 0; i < 6; ++i) {
			scale[i] = 8 * pow(2, -i / 6.0);
		}
	}
	for (int i = 0; i < 6; ++i) {
		centerDigits[i] = 0;
		sizeDigits[i] = 1;
	}
	pos = 0;
}

Box Box::child(int dir) const
{
	Box child(*this);
	child.sizeDigits[pos] *= 0.5;
	child.centerDigits[pos] += (2*dir-1)*child.sizeDigits[pos];
//	printf("digits[%d] = %f\n", pos, child.centerDigits[pos]);
	++child.pos;
	if (child.pos == 6)
		child.pos = 0;
	return child;
}

Params<Complex> Box::center() const
{
	Params<Complex> result;
	result.lattice = Complex(scale[3]*centerDigits[3], scale[0]*centerDigits[0]);
	result.loxodromicSqrt = Complex(scale[4]*centerDigits[4], scale[1]*centerDigits[1]);
	result.parabolic = Complex(scale[5]*centerDigits[5], scale[2]*centerDigits[2]);
	return result;
}

Params<Complex> Box::offset(const double* offset) const
{
	Params<Complex> result;
	result.lattice = Complex(
		scale[3]*(offset[3]*sizeDigits[3] + centerDigits[3]),
		scale[0]*(offset[0]*sizeDigits[0] + centerDigits[0])
	);
	result.loxodromicSqrt = Complex(
		scale[4]*(offset[4]*sizeDigits[4] + centerDigits[4]),
		scale[1]*(offset[1]*sizeDigits[1] + centerDigits[1])
	);
	result.parabolic = Complex(
		scale[5]*(offset[5]*sizeDigits[5] + centerDigits[5]),
		scale[2]*(offset[2]*sizeDigits[2] + centerDigits[2])
	);
	return result;
}

Params<Complex> Box::minimum() const
{
	int TODO_ULP;
	double m[6];
	for (int i = 0; i < 6; ++i) {
		if (centerDigits[i] < 0)
			m[i] = scale[i]*(centerDigits[i]+sizeDigits[i]);
		else
			m[i] = scale[i]*(centerDigits[i]-sizeDigits[i]);
	}
	
	Params<Complex> result;
	result.lattice = Complex(m[3], m[0]);
	result.loxodromicSqrt = Complex(m[4], m[1]);
	result.parabolic = Complex(m[5], m[2]);
	return result;
}

Params<Complex> Box::maximum() const
{
	int TODO_ULP;
	double m[6];
	for (int i = 0; i < 6; ++i) {
		if (centerDigits[i] < 0)
			m[i] = scale[i]*(centerDigits[i]-sizeDigits[i]);
		else
			m[i] = scale[i]*(centerDigits[i]+sizeDigits[i]);
	}
	
	Params<Complex> result;
	result.lattice = Complex(m[3], m[0]);
	result.loxodromicSqrt = Complex(m[4], m[1]);
	result.parabolic = Complex(m[5], m[2]);
	return result;
}

Params<AComplex1Jet> Box::cover() const
{
	Params<AComplex1Jet> result;
	result.lattice = AComplex1Jet(
		Complex(scale[3]*centerDigits[3], scale[0]*centerDigits[0]),
		Complex(scale[3]*sizeDigits[3], scale[0]*sizeDigits[0]),
		0.,
		0.
	);
	result.loxodromicSqrt = AComplex1Jet(
		Complex(scale[4]*centerDigits[4], scale[1]*centerDigits[1]),
		0.,
		Complex(scale[4]*sizeDigits[4], scale[1]*sizeDigits[1]),
		0.
	);
	result.parabolic = AComplex1Jet(
		Complex(scale[5]*centerDigits[5], scale[2]*centerDigits[2]),
		0.,
		0.,
		Complex(scale[5]*sizeDigits[5], scale[2]*sizeDigits[2])
	);
	return result;
}

void Box::volumeRange(double& low, double& high) const
{
	Params<Complex> min = minimum();
	Params<Complex> max = maximum();
	
	Complex minSl = min.loxodromicSqrt;
	Complex maxSl = max.loxodromicSqrt;
	low = (minSl.real()*minSl.real() + minSl.imag()*minSl.imag()) * min.lattice.imag();
	high = (maxSl.real()*maxSl.real() + maxSl.imag()*maxSl.imag()) * max.lattice.imag();
}

NamedBox NamedBox::child(int dir) const
{
	NamedBox child(Box::child(dir));
	child.name = name;
	child.name.append(1, '0'+dir);
	child.qr = qr;
	return child;
}
