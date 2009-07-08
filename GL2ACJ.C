#pragma implementation
#include "GL2ACJ.h"

  GL2ACJ::
GL2ACJ
()
  :a(1),b(0),c(0),d(1),ortho_(1),length_(1)
{
}

  GL2ACJ::
GL2ACJ
(const AComplex1Jet& aa, const AComplex1Jet& bb, const AComplex1Jet& cc, const AComplex1Jet& dd)
  :a(aa),b(bb),c(cc),d(dd), ortho_(0), length_(0)
{
}

  GL2ACJ& GL2ACJ::
operator = (const GL2ACJ& y)
{
  a = y.a;
  b = y.b;
  c = y.c;
  d = y.d;
  return *this;
}

  const AComplex1Jet GL2ACJ::
ortho
()
{
  if (ortho_ == 0)
    computeOrtho();
  return ortho_;
}

  const AComplex1Jet GL2ACJ::
length
()
{
  if (length_ == 0)
    computeLength();
  return length_;
}

  AComplex1Jet GL2ACJ::
trace
()
{
  return a + d;
}

  void GL2ACJ::
computeOrtho
()
{
  AComplex1Jet t = a*d + b*c;
  AComplex1Jet r = AComplex1Jet(sqrt(t*t - 1));
  if (norm(t + r) > 1)
    ortho_ = t + r;
  else
    ortho_ = t - r;
}

  void GL2ACJ::
computeLength
()
{
  AComplex1Jet t = (a + d)/2;
  AComplex1Jet r = AComplex1Jet(sqrt(t*t-1));
  if (norm(t+r) > 1)
    length_ = t+r;
  else
    length_ = t-r;
  length_ = length_*length_;
}

  double GL2ACJ::
distance
()
{
  return (norm(a) + norm(b) + norm(c) + norm(d))/2;
}

// This constructor gives the composition of a translation by z along
// the Z axis with a translation by x along the X axis.
// GL2ACJ(I,y,I) gives translation by y along the Y axis.
// If |x| > 1 and |z| > 1, then we will have:
// length(GL2ACJ(z,0,0)) == length(GL2ACJ(0,z,0)) = length(GL2ACJ(0,0,z)) == z
// ortho(GL2ACJ(z1,x,z2)) == x

  GL2ACJ
generator
(const AComplex1Jet& z1, const AComplex1Jet& x, const AComplex1Jet& z2)
{
  AComplex1Jet sx = sqrt(x), sz1 = sqrt(z1), sz2 = sqrt(z2);
  AComplex1Jet sh = (sx-1/sx)/2;
  AComplex1Jet ch = (sx+1/sx)/2;
  return GL2ACJ(ch*sz1*sz2, sh*sz1/sz2, sh/sz1*sz2, ch/sz1/sz2);
}

// shortGenerator(z) == generator(z,1,1).

  GL2ACJ
shortGenerator
(const AComplex1Jet& z)
{
  AComplex1Jet sz = sqrt(z);
  AComplex1Jet zero(0);
  return GL2ACJ(sz,zero,zero,1/sz);
}

// closeGenerator(x,z) == generator(1,x,z).

  GL2ACJ
closeGenerator
(const AComplex1Jet& x, const AComplex1Jet& z)
{
  AComplex1Jet sx = sqrt(x), sz = sqrt(z);
  AComplex1Jet sh = (sx-1/sx)/2;
  AComplex1Jet ch = (sx+1/sx)/2;
  return GL2ACJ(ch*sz, sh/sz, sh*sz, ch/sz);
}

  GL2ACJ
operator *
(const GL2ACJ& l, const GL2ACJ& r)
{
  return GL2ACJ(
    l.a*r.a+l.b*r.c, l.a*r.b+l.b*r.d, 
    l.c*r.a+l.d*r.c, l.c*r.b+l.d*r.d
  );
}

  GL2ACJ
operator *=
(GL2ACJ& x, const GL2ACJ& y)
{
  x = x*y;
  return x;
}

  GL2ACJ GL2ACJ::
operator ~ () const
{
  return GL2ACJ(d, -b, -c, a);
}

