#pragma implementation
#include "AComplex1Jet.h"
#include <stdio.h>

// maximum error happens when |x| and |y| are both maximum.
AComplex1Jet
operator *
(const AComplex1Jet& x, const AComplex1Jet& y)
{
  double xdist = x.size();
  double ydist = y.size();
  double ax = abs(x.f), ay = abs(y.f);
  return AComplex1Jet(
    x.f*y.f,
    x.f*y.df0 + x.df0*y.f, x.f*y.df1 + x.df1*y.f, x.f*y.df2 + x.df2*y.f,
    (ax+xdist+x.error())*(ay+ydist+y.error()) - (ax*ay+xdist*ay+ydist*ax)
  );
}

// maximum error happens when |x| is maximum and |y| is minumum.
AComplex1Jet
operator /
(const AComplex1Jet& x, const AComplex1Jet& y)
{
  double xdist = x.size();
  double ydist = y.size();
  double ax = abs(x.f), ay = abs(y.f);
  if (y.error() + ydist >= ay) {
    return AComplex1Jet(INFINITY,0,0,0,0);
  }
  return AComplex1Jet(
    x.f/y.f,
    (x.df0*y.f - x.f*y.df0)/(y.f*y.f),
    (x.df1*y.f - x.f*y.df1)/(y.f*y.f),
    (x.df2*y.f - x.f*y.df2)/(y.f*y.f),
    (ax+xdist+x.error())/(ay-ydist-y.error()) - (ax/ay+xdist/ay+ydist*ax/(ay*ay))
  );
}

AComplex1Jet
operator /
(double x, const AComplex1Jet& y)
{
  double ydist = y.size();
  double ax = fabs(x), ay = abs(y.f);
  if (y.error() + ydist >= ay) {
    return AComplex1Jet(INFINITY,0,0,0,0);
  }
  return AComplex1Jet(
    x/y.f,
    -x*y.df0/y.f / y.f,
    -x*y.df1/y.f / y.f,
    -x*y.df2/y.f / y.f,
    (ax/(ay-ydist-y.error())) - (ax/ay + ydist*ax/(ay*ay))
  );
}

// maximum error is when |x| is minimum
AComplex1Jet
sqrt
(const AComplex1Jet& x)
{
  double xdist = x.size();
  double ax = abs(x.f);
  Complex s = sqrt(x.f);
  double ax_min = ax - xdist - x.error();
  if (ax_min > 0) {
    return AComplex1Jet(
      s, x.df0/(2.0*s), x.df1/(2.0*s), x.df2/(2.0*s),
      sqrt(ax) - xdist/(2*sqrt(ax)) - sqrt(ax-xdist-x.error()) 
    );
  } else {
    return AComplex1Jet(0, 0, 0, 0, sqrt(ax + xdist + x.error()));
  }
}

// maximum error when real(x) is maximum
AComplex1Jet
exp
(const AComplex1Jet& x)
{
  double xdist = x.size();
  double ax =  real(x.f);
  Complex e = exp(x.f);
  return AComplex1Jet(
    e, e*x.df0, e*x.df1, e*x.df2,
    exp(ax+xdist+x.error()) - (1+xdist)*exp(ax) 
  );
}
