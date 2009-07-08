/*
 *  CanonicalName.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 12/10/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CanonicalName.h"
#include <set>

using namespace std;

int gPower(string s)
{
	string::size_type p;
	int gCount = 0;
	for (p = 0; p < s.size(); ++p)
		if (s[p] == 'g' || s[p] == 'G')
			++gCount;
	return gCount;
}

char invertLetter(char c)
{
	switch (c) {
		case 'g': return 'G';
		case 'G': return 'g';
		case 'm': return 'M';
		case 'M': return 'm';
		case 'n': return 'N';
		case 'N': return 'n';
		default: return '?';
	}
}

string CanonicalName::inverse(string s)
{
	reverse(s.begin(), s.end());
	for (string::size_type sp = 0; sp < s.size(); ++sp)
		s[sp] = invertLetter(s[sp]);
	return s;
}


CanonicalName::CanonicalName()
{
	substitutions.push_back(Substitution("nm", "mn"));
	substitutions.push_back(Substitution("nM", "Mn"));
	substitutions.push_back(Substitution("Nm", "mN"));
	substitutions.push_back(Substitution("NM", "MN"));
	substitutions.push_back(Substitution("nN", ""));
	substitutions.push_back(Substitution("Nn", ""));
	substitutions.push_back(Substitution("mM", ""));
	substitutions.push_back(Substitution("Mm", ""));
	substitutions.push_back(Substitution("gG", ""));
	substitutions.push_back(Substitution("Gg", ""));
}

void CanonicalName::addSubstitution(string& a, string& b)
{
//	printf("addSub(%s,%s)\n", a.c_str(), b.c_str());
	string ac = reduce(a);
	string bc = reduce(b);
	if (ac == bc)
		return;
	int ap = gPower(ac);
	int bp = gPower(bc);
	if (ap < bp) {
		if (getClass(ac) == bc)
			substitutions.push_back(Substitution(bc, ac));
	} else if (ap > bp) {
		if (getClass(ac) == ac)
			substitutions.push_back(Substitution(ac, bc));
	} else {
		string aclass = getClass(ac);
		string bclass = getClass(bc);
		if (aclass < bclass) {
			if (bclass == bc)
				substitutions.push_back(Substitution(bc, ac));
		} else if (bclass < aclass) {
			if (aclass == ac)
				substitutions.push_back(Substitution(ac, bc));
		}
		
		return;
	}
//	printf("added %s -> %s\n", substitutions.back().s.c_str(),
//		substitutions.back().rep.c_str());
}

void CanonicalName::addRelator(string relator)
{
	string rr(relator + relator);
	string::size_type l = relator.size();
	string::size_type sl;
	string::size_type pos;
	for (pos = 0; pos < l; ++pos) {
		for (sl = 1; sl < l; ++sl) {
			string a(rr.substr(pos, sl));
			string b(inverse(rr.substr(pos+sl, l-sl)));
			addSubstitution(a, b);
		}
	}
}

string CanonicalName::reduce(string s)
{
	set<string> visited;
	bool done = false;
	while (!done) {
		done = true;
		vector<Substitution>::iterator it;
		for (it = substitutions.begin(); it != substitutions.end(); ++it) {
			string::size_type pos = s.find(it->s);
			if (pos != string::npos) {
				done = false;
				s.replace(pos, it->s.length(), it->rep);
				if (visited.find(s) != visited.end()) {
					fprintf(stderr, "loop detected in %s\n", s.c_str());
					return s;
				}
				visited.insert(s);
			}
		}
	}
	return s;
}

string CanonicalName::getClass(string& s)
{
	string::size_type firstG = s.find_first_of("gG");
	string::size_type lastG = s.find_last_of("gG");
	if (firstG != string::npos)
		return s.substr(firstG, lastG - firstG + 1);
	else
		return "";
}

string CanonicalName::getCanonicalName(string s)
{
	s = reduce(inverse(reduce(inverse(s))));
	return s;
}

string CanonicalName::getCanonicalClass(string s)
{
	string cs = getCanonicalName(s);
	return getClass(cs);
}