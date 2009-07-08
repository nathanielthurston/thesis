/*
 *  CanonicalName.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 12/10/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <vector>
#include <map>

class CanonicalName {
public:
	CanonicalName();
	std::string inverse(std::string name);
	void addRelator(std::string relator);
	std::string getCanonicalName(std::string name);
	std::string getCanonicalClass(std::string name);
	
private:
	void addSubstitution(std::string& a, std::string& b);
	std::string reduce(std::string s);
	std::string getClass(std::string& s);
	void addRelatorInternal(std::string relator);
	void initImpl();
	void initSubstitutions();
	struct Substitution {
		Substitution() {}
		Substitution(std::string s_, std::string rep_) :s(s_), rep(rep_) {}
		std::string s;
		std::string rep;
	};
	
	std::vector<Substitution> substitutions;

	std::vector<std::string> relators;
	CanonicalName* impl;
	static std::map<std::string, CanonicalName*> cache;
};
