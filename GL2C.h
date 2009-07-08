#ifndef _GL2C_h_
#define _GL2C_h_
#ifdef __GNUG__
#pragma interface
#endif
#include "Complex.h"
class GL2C {
  public:

  GL2C();
  GL2C(const Complex&, const Complex&, const Complex&, const Complex&);
  GL2C& operator = (const GL2C& y);
  const Complex ortho();
  const Complex length();
  Complex trace();
  double distance();
  friend GL2C generator(const Complex&, const Complex&, const Complex&);
  friend GL2C shortGenerator(const Complex&);
  friend GL2C closeGenerator(const Complex&, const Complex&);
  friend GL2C operator*(const GL2C& left, const GL2C& right);
  GL2C operator ~ () const;
  Complex a, b, c, d;

  protected:
  Complex ortho_;
  Complex length_;
  void computeOrtho();
  void computeLength();
};

GL2C operator *= (GL2C& x, const GL2C& y);
#endif
