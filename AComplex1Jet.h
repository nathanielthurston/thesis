#ifndef _AComplex1Jet_h_
#define _AComplex1Jet_h_
#ifdef __GNUG__
#pragma interface
#endif
#include <assert.h>
#include "Complex.h"

// An AComplex1Jet represents a function f, with domain the unit hypercube
// (z0,z1,z2) : |Re(z0)|<1, |Im(z0)|<1, |Re(z1)|<1, |Im(z1)|<1, |Re(z1)|<1, |Im(z2)<1
// such that |f(z) - f - (df0,df1,df2)(z)| < error.
// In practice, f == f(0) and f'(0) == (df0,df1,df2)

class AComplex1Jet
{
  public:
  AComplex1Jet(const AComplex1Jet&);
  explicit AComplex1Jet(const Complex& ff=0, const Complex& dff0=0, const Complex& dff1=0, const Complex& dff2=0, double e=0);
  Complex center() const;
  double size() const;
  void computeSize();
  double error() const;

  AComplex1Jet& operator = (const AComplex1Jet& y);
  friend AComplex1Jet operator * (const AComplex1Jet&, const AComplex1Jet&);
  friend AComplex1Jet operator + (const AComplex1Jet&, const AComplex1Jet&);
  friend AComplex1Jet operator + (const AComplex1Jet&, double);
  friend AComplex1Jet operator - (const AComplex1Jet&, const AComplex1Jet&);
  friend AComplex1Jet operator - (const AComplex1Jet&, double);
  friend AComplex1Jet operator - (const AComplex1Jet&, Complex);
  friend AComplex1Jet operator - (const AComplex1Jet&);
  friend AComplex1Jet operator / (const AComplex1Jet&, const AComplex1Jet&);
  friend AComplex1Jet operator / (const AComplex1Jet&, double);
  friend AComplex1Jet operator / (double, const AComplex1Jet&);
  friend int operator == (const AComplex1Jet&, int);
  friend AComplex1Jet sqrt(const AComplex1Jet&);
  friend AComplex1Jet sqrtNearZero(const AComplex1Jet&);
  friend AComplex1Jet exp(const AComplex1Jet&);
  friend double norm(const AComplex1Jet&);
  friend Complex D(int index, const AComplex1Jet&);
  friend double maxabs(const AComplex1Jet&);
  friend double minabs(const AComplex1Jet&);

  private:
  Complex f;
  Complex df0,df1,df2;
  double error_;
  double size_;
};

inline AComplex1Jet operator *= (AComplex1Jet&, const AComplex1Jet&);

inline AComplex1Jet::
AComplex1Jet(const Complex& ff, const Complex& dff0, const Complex& dff1, const Complex& dff2, double e)
  :f(ff),df0(dff0),df1(dff1),df2(dff2), error_(e)
{
  computeSize();
}

inline AComplex1Jet::
AComplex1Jet(const AComplex1Jet& x)
  :f(x.f),df0(x.df0),df1(x.df1),df2(x.df2),error_(x.error())
{
  computeSize();
}

inline AComplex1Jet& AComplex1Jet::
operator = (const AComplex1Jet& y)
{
  f = y.f; df0 = y.df0; df1 = y.df1; df2 = y.df2;
  size_ = y.size(); error_ = y.error();
  return *this;
}

inline Complex AComplex1Jet::
center() const
{
  return f;
}

inline void AComplex1Jet::
computeSize()
{
  size_ = abs(df0) + abs(df1) + abs(df2);
}

inline double AComplex1Jet::
size() const
{
  return size_;
//  return abs(df0) + abs(df1) + abs(df2);
}

inline double AComplex1Jet::
error() const
{
  return error_;
}

inline Complex
D(int index, const AComplex1Jet& x)
{
  switch(index) {
    case 0: return x.df0;
    case 1: return x.df1;
    case 2: return x.df2;
    default: assert(0); return x.df0;
  }
}

inline double
maxabs(const AComplex1Jet& x)
{
  return abs(x.f) + x.size() + x.error();
}

inline double
minabs(const AComplex1Jet& x)
{
  double v = abs(x.f) - x.size() - x.error();
  return (v > 0) ? v : 0;
}

inline AComplex1Jet
operator +
(const AComplex1Jet& x, const AComplex1Jet& y)
{
  return AComplex1Jet(
    x.f+y.f, x.df0+y.df0, x.df1+y.df1, x.df2+y.df2, x.error() + y.error()
  );
}

inline AComplex1Jet
operator +
(const AComplex1Jet& x, double y)
{
  return AComplex1Jet( x.f+y, x.df0,x.df1,x.df2, x.error());
}

inline AComplex1Jet
operator -
(const AComplex1Jet& x, const AComplex1Jet& y)
{
  return AComplex1Jet(
    x.f-y.f, x.df0-y.df0, x.df1-y.df1, x.df2-y.df2, x.error() + y.error()
  );
}

inline AComplex1Jet
operator -
(const AComplex1Jet& x, double y)
{
  return AComplex1Jet( x.f-y, x.df0,x.df1,x.df2, x.error());
}

inline AComplex1Jet
operator -
(const AComplex1Jet& x, Complex y)
{
  return AComplex1Jet( x.f-y, x.df0,x.df1,x.df2, x.error());
}

inline AComplex1Jet
operator -
(const AComplex1Jet& x)
{
  return AComplex1Jet(-x.f, -x.df0, -x.df1, -x.df2, x.error());
}

inline AComplex1Jet
operator /
(const AComplex1Jet& x, double y)
{
  return AComplex1Jet(x.f/y, x.df0/y, x.df1/y, x.df2/y, x.error()/y);
}

inline double norm(const AComplex1Jet& x)
{
  return norm(x.f);
}

inline int
operator ==
(const AComplex1Jet& x, int y)
{
  return x.f == double(y) && x.df0 == 0.0 && x.df1 == 0.0 && x.df2 == 0.0 && x.error() == 0.0;
}

inline AComplex1Jet
operator *= (AComplex1Jet& x, const AComplex1Jet& y)
{
  x = x*y;
  return x;
}

inline AComplex1Jet operator * (int n, const AComplex1Jet& x)
{
	AComplex1Jet r(0.);
	while (n > 0) {
		r = r + x;
		--n;
	}
	while (n < 0) {
		r = r - x;
		++n;
	}
	return r;
}


#endif
