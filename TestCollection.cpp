/*
 *  TestCollection.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */
// The bounds:
// 0. |sl| >= 1 (horoball size)
// 1. Im(sl) >= 0 (only sl^2 matters)
// 2. -1/2 <= Re(n) <= 1/2 (reduction modulo M)
// 3. Im(n) >= 0 (complex conjugate symmetry)
// 4. 0 <= Im(p) <= Im(n)/2 (reduction modulo N, flipping sign of N)
// 5. 0 <= Re(p) <= 1/2 (reduction modulo M, flipping sign of M)
// 6. |sl^2| Im(n) <= 4 (area of fundamental paralleogram)
// 
#include <stdio.h>
#include "TestCollection.h"
#include "ImpossibleRelations.h"
using namespace std;
using namespace __gnu_cxx;

double g_maximumArea = 5.1;
int g_xLattice;
int g_yLattice;

string g_testCollectionFullWord;

int TestCollection::size()
{
	return 7 + indexString.size();
}

string checkPower(string word, int x, int y)
{
	char buf[200];
	char *bp = buf;
	if (abs(x) > 10 || abs(y) > 10)
		return "";
	while (x > 0) { *bp++ = 'm'; --x; }
	while (x < 0) { *bp++ = 'M'; ++x; }
	while (y > 0) { *bp++ = 'n'; --y; }
	while (y < 0) { *bp++ = 'N'; ++y; }
	strcpy(bp, word.c_str());
	int l = strlen(buf);
	for (int n = 2; n+n <= l; ++n) {
		int k;
		for ( k = 1; k*n < l; ++k) ;
		if (k*n == l) {
			for (--k; k > 0; --k) {
				if (strncmp(buf, buf+k*n, n))
					break;
			}
			if (k == 0) {
//				fprintf(stderr, "id %s x%d\n", buf, n);
				g_testCollectionFullWord = buf;
				buf[n] = '\0';
				return buf;
			}
		}
	}
//	fprintf(stderr, "fullWord = %s\n", buf);
	return "";
}

bool TestCollection::evaluate(string word, Params<Complex>& params)
{
	GL2C w(1,0,0,1);
	GL2C G(constructG(params));
	GL2C g(~G);
	
	string::size_type pos;
	int x = 0;
	int y = 0;
	for (pos = 0; pos <= word.length(); ++pos) {
		int c = pos < word.length() ? word[pos] : -1;
		switch(c) {
			case 'm': --x; break;
			case 'M': ++x; break;
			case 'n': --y; break;
			case 'N': ++y; break;
			default: {
				if (x != 0 || y != 0) {
					w = w*constructT(params, x, y);
					x=y=0;
				}
				if (c == 'g')
					w = w*g;
				else if (c == 'G')
					w = w*G;
			}
		}
	}
	return abs(w.c/g.c) < 1;
}

GL2ACJ TestCollection::evaluate1(string word, Params<AComplex1Jet>& params)
{
	AComplex1Jet one(1), zero(0);
	GL2ACJ w(one, zero, zero, one);
	GL2ACJ G(constructG(params));
	GL2ACJ g(~G);
	
	string::size_type pos;
	int x = 0;
	int y = 0;
	for (pos = 0; pos <= word.length(); ++pos) {
		int c = pos < word.length() ? word[pos] : -1;
		switch(c) {
			case 'm': --x; break;
			case 'M': ++x; break;
			case 'n': --y; break;
			case 'N': ++y; break;
			default: {
				if (x != 0 || y != 0) {
					w = w*constructT(params, x, y);
					x=y=0;
				}
				if (c == 'g')
					w = w*g;
				else if (c == 'G')
					w = w*G;
			}
		}
	}
	return w;
}

bool TestCollection::validIdentity(string word, int xLattice, int yLattice, Params<AComplex1Jet>& params)
{
	while (xLattice > 0) {
		--xLattice;
		word += "m";
	}
	while (xLattice < 0) {
		++xLattice;
		word += "M";
	}
	while (yLattice > 0) {
		--yLattice;
		word += "n";
	}
	while (yLattice < 0) {
		++yLattice;
		word += "N";
	}
	for (string::size_type pos = 0; pos < word.size(); ++pos) {
		string pword = word.substr(pos, word.size()-pos) + word.substr(0, pos);
		GL2ACJ w(evaluate1(pword, params));
		if (minabs(w.a-1) > 0 && minabs(w.a+1) > 0
		 || minabs(w.b) > 0
		 || minabs(w.c) > 0
		 || minabs(w.d-1) > 0 && minabs(w.d+1) > 0)
			return false;
	}
	return true;
}

int TestCollection::evaluate(string word, Params<AComplex1Jet>& params, bool isNotIdentity)
{
	GL2ACJ w(evaluate1(word, params));
	GL2ACJ G(constructG(params));
	GL2ACJ g(~G);
	if (isNotIdentity) {
		return minabs(w.b) > 0 || minabs(w.c) > 0
		|| minabs(w.a-1) > 0 && minabs(w.a+1) > 0
		|| minabs(w.d-1) > 0 && minabs(w.d+1) > 0;
	}
	AComplex1Jet r(w.c / g.c);
	if (maxabs(r) < 1) {
		if (minabs(w.c) > 0
		 || minabs(w.a-1) > 0 && minabs(w.a+1) > 0
		 || minabs(w.d-1) > 0 && minabs(w.d+1) > 0) {
			return 1;
		} else {
			list<string> mandatory;
			bool isImpossible = impossible->isAlwaysImpossible(word, mandatory);
			if (isImpossible)
				return 3;
				
			Complex t = w.b.center() / w.a.center();
			int y = int(floor(t.imag() / params.lattice.center().imag()));
			int numFound = 0;
			int xLattice, yLattice;
			for (int yi = -1; yi <= 2; ++yi) {
				Complex tY = t - double(y+yi) * params.lattice.center();
				int x = int(floor(tY.real()));
				for (int xi = -1; xi <= 2; ++xi) {
					AComplex1Jet tL = w.b - w.a*((y+yi) * params.lattice + (x+xi));
					if (minabs(tL) == 0) {
						++numFound;
						if (numFound > 1)
							break;
						xLattice = x + xi;
						yLattice = y + yi;
					}
				}
			}
			
			if (numFound == 0) { // cannot be on lattice
				return 4;
			}
			if (numFound == 1) { // exactly one lattice point possible
				// TODO: prove this, and possibly get better bounds on the number
				// of points which need to be checked
				isImpossible = impossible->isImpossible(word, xLattice, yLattice, mandatory);
				if (isImpossible) {
					return 3;
				} else {
					g_xLattice = xLattice;
					g_yLattice = yLattice;
					if (validIdentity(word, xLattice, yLattice, params))
						return 6;
					else
						return 7;
				}
			}
			for (list<string>::iterator it = mandatory.begin(); it != mandatory.end(); ++it) {
				if (evaluate(*it, params, true))
					return 5;
			}
			return 2;
		}
	} else {
		return 0;
	}
}

double g_latticeArea;
bool TestCollection::evaluateCenter(int index, Box& box)
{
	Params<Complex> params = box.center();
	switch(index) {
		case 0:	{
			Complex sl = params.loxodromicSqrt;
			return sl.real()*sl.real() + sl.imag()*sl.imag() <= 1;
		}
		case 1: return params.loxodromicSqrt.imag() <= 0
		 || params.lattice.imag() <= 0
		 || params.parabolic.imag() <= 0
		 || params.parabolic.real() <= 0;
		case 2: return abs(params.lattice.real()) > 0.5;
		case 3: return norm(params.lattice) < 1;
		case 4: return params.parabolic.imag() >= 0.5*params.lattice.imag();
		case 5: return params.parabolic.real() >= 0.5;
		case 6: {
			g_latticeArea = norm(params.loxodromicSqrt)*params.lattice.imag();
			return g_latticeArea >= g_maximumArea;
		}
		default:
			return evaluate(indexString[index-7], params);
	}
}

int TestCollection::evaluateBox(int index, Box& box)
{
	int TODO_ULP;
	Params<Complex> minimum = box.minimum();
	Params<Complex> maximum = box.maximum();
	enumerate("");
	switch(index) {
		case 0: {
			Complex maxSl = maximum.loxodromicSqrt;
			return maxSl.real()*maxSl.real() + maxSl.imag()*maxSl.imag() <= 1;
		}
		case 1: return maximum.loxodromicSqrt.imag() <= 0
		 || maximum.lattice.imag() <= 0
		 || maximum.parabolic.imag() <= 0
		 || maximum.parabolic.real() <= 0;
		case 2: return abs(minimum.lattice.real()) > 0.5;
		case 3: return norm(maximum.lattice) < 1;
		case 4: return minimum.parabolic.imag() > 0.5*maximum.lattice.imag();
		case 5: return minimum.parabolic.real() > 0.5;
		case 6: {
			Params<AComplex1Jet> cover(box.cover());
			double absLS = minabs(cover.loxodromicSqrt);
			double area = absLS * absLS * minimum.lattice.imag();
			if (area > g_maximumArea) {
				return true;
			} else {
				return false;
			}
		}
		default: {
			Params<AComplex1Jet> cover(box.cover());
			int result = evaluate(indexString[index-7], cover);
			if (result) {
				enumerate(indexString[index-7].c_str());
			}
			return result;
		}
	}
}

const char* TestCollection::getName(int index)
{
	static char buf[4];
	if (index < 7) {
		sprintf(buf, "%d", index);
		return buf;
	} else {
		return indexString[index-7].c_str();
	}
}


int TestCollection::add(string word)
{
	map<string, int>::iterator it = stringIndex.find(word);
	if (it == stringIndex.end()) {
//		printf("adding %d=%s\n", indexString.size(), word.c_str());
		stringIndex[word] = indexString.size();
		indexString.push_back(word);
		return indexString.size()+6;
	}
	else
		return it->second+7;
}

int g_maxWordLength = 2;
void TestCollection::enumerate(const char* w)
{
	static vector<int> maxP;
	if (!*w && !maxP.empty())
		return;
	int pCount=0, lCount=0;
	while (*w) {
		if (*w == 'g' || *w == 'G')
			++lCount;
		else
			++pCount;
		++w;
	}
	if (lCount == 0) lCount = 1;
	if (lCount >= maxP.size()) {
		maxP.resize(lCount+1, -1);
	}
	if (pCount <= maxP[lCount]) {
		return;
	}
	
	if (lCount + pCount > g_maxWordLength)
		g_maxWordLength = lCount + pCount;
	
	maxP[lCount] = pCount;
	
	if (pCount > 2)
		pCount = 2;
	if (lCount > 2)
		lCount = 2;
//	printf("ENUMERATING %d,%d\n", pCount, lCount);
//	enumerateTails("", pCount, lCount);
}

void TestCollection::enumerateTails(string s, int pCount, int lCount)
{
	if (pCount < -1 || lCount < -1 || (pCount == -1 && lCount == -1))
		return;
	const char* p = "Gg";
	if (s.size() > 0) {
		char last = s[s.size()-1];
		switch(last) {
			case 'G': p = "GMmNn"; break;
			case 'g': p = "gMmNn"; break;
			case 'M': p = "GgMNn"; break;
			case 'm': p = "GgmNn"; break;
			case 'N': p = "GgN"; break;
			case 'n': p = "Ggn"; break;
		}
	}
	for (; *p; ++p) {
		string n = s;
		n.append(1, *p);
		if (*p == 'g' || *p == 'G') {
			add(n);
			enumerateTails(n, pCount, lCount-1);
		} else {
			enumerateTails(n, pCount-1, lCount);
		}
	}
}

void TestCollection::load(const char* fileName)
{
	FILE *fp = fopen(fileName, "r");
	char buf[1024];
	while (fp && fgets(buf, sizeof(buf), fp)) {
		int n = strlen(buf);
		if (!isalpha(buf[n-1]))
			--n;
		add(string(buf, n));
	}
}

void TestCollection::loadImpossibleRelations(const char* fileName)
{
	impossible = ImpossibleRelations::create(fileName);
}
