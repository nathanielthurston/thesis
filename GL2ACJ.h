#ifndef _GL2ACJ_h_
#define _GL2ACJ_h_
#ifdef __GNUG__
#pragma interface
#endif
#include "AComplex1Jet.h"
class GL2ACJ {
  public:

  GL2ACJ();
  GL2ACJ(const AComplex1Jet&, const AComplex1Jet&, const AComplex1Jet&, const AComplex1Jet&);
  GL2ACJ& operator = (const GL2ACJ& y);
  const AComplex1Jet ortho();
  const AComplex1Jet length();
  AComplex1Jet trace();
  double distance();
  friend GL2ACJ generator(const AComplex1Jet&, const AComplex1Jet&, const AComplex1Jet&);
  friend GL2ACJ shortGenerator(const AComplex1Jet&);
  friend GL2ACJ closeGenerator(const AComplex1Jet&, const AComplex1Jet&);
  friend GL2ACJ operator*(const GL2ACJ& left, const GL2ACJ& right);
  GL2ACJ operator ~ () const;

  AComplex1Jet a, b, c, d;
  protected:
  AComplex1Jet ortho_;
  AComplex1Jet length_;
  void computeOrtho();
  void computeLength();
};

GL2ACJ operator *= (GL2ACJ& x, const GL2ACJ& y);
#endif
