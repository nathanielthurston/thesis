/*
 *  Evaluate.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 09/10/2007.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Box.h"
#include "ImpossibleRelations.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <list>
using namespace std;

vector<string> identities;
list<string> allMandatory;
ImpossibleRelations *impossible;

void print(const AComplex1Jet& j, const char* what) {
	Complex c = j.center();
	Complex d0 = D(0, j);
	Complex d1 = D(1, j);
	Complex d2 = D(2, j);
	printf("%5s = %8.3f  %8.3f  %8.3f  %8.3f\n", what, c.real(), d0.real(), d1.real(), d2.real());
	printf("%5s   %8.3fI %8.3fI %8.3fI %8.3fI\n", "", c.imag(), d0.imag(), d1.imag(), d2.imag());
	printf("%5s err=%8.3g size=%8.3g abs=[%8.3g, %8.3g]\n",
		"", j.error(), j.size(), minabs(j), maxabs(j));
}

bool getID(const char* wordBuf, const char* boxBuf)
{
	Box box;
	const char* bnp;
	for ( bnp = boxBuf; *bnp; ++bnp) {
		if (*bnp == '0') {
			box = box.child(0);
		} else if (*bnp == '1') {
			box = box.child(1);
		} else {
			printf("unexpected box character '%c'", *bnp);
			break;
		}
	}
	
	Params<AComplex1Jet> params = box.cover();
	
	GL2ACJ G(constructG(params));
	GL2ACJ g = ~G;
	GL2ACJ M(constructT(params,1,0));
	GL2ACJ m(constructT(params,-1,0));
	GL2ACJ N(constructT(params, 0, 1));
	GL2ACJ n(constructT(params, 0, -1));
	
	AComplex1Jet zero(0.), one(1.);
	GL2ACJ w(one,zero,zero,one);
	
	for (const char* wp = wordBuf; *wp; ++wp) {
		switch(*wp) {
			case 'g': w = w*g; break;
			case 'G': w = w*G; break;
			case 'm': w = w*m; break;
			case 'M': w = w*M; break;
			case 'n': w = w*n; break;
			case 'N': w = w*N; break;
		}
	}
	if (maxabs(w.c/g.c) > 1) {
		fprintf(stderr, "%s(%s) has a small horoball\n", wordBuf, boxBuf);
		return false;
	}
	if (minabs(w.c) > 0) {
		fprintf(stderr, "%s(%s) not parabolic\n", wordBuf, boxBuf);
		return true;
	}
	list<string> mandatory;
	bool isImpossible = impossible->isAlwaysImpossible(wordBuf, mandatory);
	if (isImpossible) {
		fprintf(stdout, "%s(%s) never possible\n", wordBuf, boxBuf);
		return true;
	}
	Complex t = w.b.center() / w.a.center();
	int y = int(floor(t.imag() / params.lattice.center().imag()));
	int numFound = 0;
	int xLattice, yLattice;
	for (int yi = 0; yi <= 1; ++yi) {
		Complex tY = t - double(y+yi) * params.lattice.center();
		int x = int(floor(tY.real()));
		for (int xi = 0; xi <= 1; ++xi) {
			AComplex1Jet tL = w.b - w.a*((y+yi) * params.lattice + (x+xi));
			if (minabs(tL) == 0) {
				++numFound;
				xLattice = x + xi;
				yLattice = y + yi;
			}
		}
	}
	if (numFound == 0) {
		printf("%s(%s) Off Lattice\n", wordBuf, boxBuf);
		return true;
	} else if (numFound > 1) {
		printf("%s(%s) Multiple Lattice Points\n", wordBuf, boxBuf);
		allMandatory.splice(allMandatory.end(), mandatory);
		return false;
	} else {
		string id(wordBuf);
		if (xLattice > 0) {
			id.append(xLattice, 'm');
		} else if (xLattice < 0) {
			id.append(-xLattice, 'M');
		}
		if (yLattice > 0) {
			id.append(yLattice, 'n');
		} else if (yLattice < 0) {
			id.append(-yLattice, 'N');
		}
		isImpossible = impossible->isImpossible(wordBuf, xLattice, yLattice, mandatory);
		if (isImpossible) {
			printf("%s not possible\n", id.c_str());
			return true;
		}
		allMandatory.splice(allMandatory.end(), mandatory);
//		printf("x=%d y=%d id=%s\n", xLattice, yLattice, id.c_str());
		identities.push_back(id);
	}
	return false;
}

int main(int argc, char** argv)
{
	char lineBuf[10000];
	char boxBuf[10000];
	char wordBuf[10000];
	impossible = ImpossibleRelations::create("/Users/njt/projects/mom/wordpowers.out");
	while (gets(lineBuf)) {
		int n = sscanf(lineBuf, "%s %s", boxBuf, wordBuf);
		if (n < 2) break;
		char* wp;
		char* commaP;
		bool foundContradiction = false;
		for (wp = wordBuf; wp; wp = commaP) {
			commaP = index(wp, ',');
			if (commaP)
				*commaP++ = '\0';
			if (getID(wp, boxBuf)) {
				foundContradiction = true;
				break;
			}
		}
		if (!foundContradiction) {
			printf("%s", boxBuf);
			for (int i = 0; i < identities.size(); ++i) {
				printf(" %s", identities[i].c_str());
			}
			for (list<string>::iterator it = allMandatory.begin(); it != allMandatory.end(); ++it) {
				printf(" =%s", it->c_str());
			}
			printf("\n");
		}
		identities.clear();
		allMandatory.clear();
	}
}
