#pragma implementation
#include "GL2C.h"
GL2C::
GL2C
()
  :a(1),b(0),c(0),d(1),ortho_(1), length_(1)
{
}

GL2C::
GL2C
(const Complex& aa, const Complex& bb, const Complex& cc, const Complex& dd)
  :a(aa),b(bb),c(cc),d(dd), ortho_(0), length_(0)
{
}

GL2C& GL2C::
operator = (const GL2C& y)
{
  a = y.a;
  b = y.b;
  c = y.c;
  d = y.d;
  return *this;
}

const Complex GL2C::
ortho
()
{
  if (ortho_ == 0.0)
    computeOrtho();
  return ortho_;
}

const Complex GL2C::
length
()
{
  if (length_ == 0.0)
    computeLength();
  return length_;
}

Complex GL2C::
trace
()
{
  return a + d;
}

void GL2C::
computeOrtho
()
{
  Complex t = a*d + b*c;
  Complex r = Complex(sqrt(t*t - 1.0));
  if (norm(t + r) > 1)
    ortho_ = t + r;
  else
    ortho_ = t - r;
}

void GL2C::
computeLength
()
{
  Complex t = (a + d)*0.5;
  Complex r = Complex(sqrt(t*t-1.0));
  if (norm(t+r) > 1)
    length_ = t+r;
  else
    length_ = t-r;
  length_ = length_*length_;
}

double GL2C::
distance
()
{
  return (norm(a) + norm(b) + norm(c) + norm(d))/2;
}

// This constructor gives the composition of a translation by z along
// the Z axis with a translation by x along the X axis.
// GL2(I,y,I) gives translation by y along the Y axis.
// If |x| > 1 and |z| > 1, then we will have:
// length(GL2(z,0,0)) == length(GL2(0,z,0)) = length(GL2(0,0,z)) == z
// ortho(GL2(z1,x,z2)) == x

GL2C
generator
(const Complex& z1, const Complex& x, const Complex& z2)
{
  Complex sx = sqrt(x), sz1 = sqrt(z1), sz2 = sqrt(z2);
  Complex sh = (sx-1.0/sx)*0.5;
  Complex ch = (sx+1.0/sx)*0.5;
  return GL2C(ch*sz1*sz2, sh*sz1/sz2, sh/sz1*sz2, ch/sz1/sz2);
}

// shortGenerator(z) == generator(z,1,1).

GL2C
shortGenerator
(const Complex& z)
{
  Complex sz = sqrt(z);
  Complex zero(0);
  return GL2C(sz,zero,zero,1.0/sz);
}

// closeGenerator(x,z) == generator(1,x,z).

GL2C
closeGenerator
(const Complex& x, const Complex& z)
{
  Complex sx = sqrt(x), sz = sqrt(z);
  Complex sh = (sx-1.0/sx)*0.5;
  Complex ch = (sx+1.0/sx)*0.5;
  return GL2C(ch*sz, sh/sz, sh*sz, ch/sz);
}

GL2C
operator *
(const GL2C& l, const GL2C& r)
{
  return GL2C(
    l.a*r.a+l.b*r.c, l.a*r.b+l.b*r.d, 
    l.c*r.a+l.d*r.c, l.c*r.b+l.d*r.d
  );
}

GL2C
operator *=
(GL2C& x, const GL2C& y)
{
  x = x*y;
  return x;
}

GL2C GL2C::
operator ~ () const
{
  return GL2C(d, -b, -c, a);
}
