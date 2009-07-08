/*
 *  TestCollection.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include <ext/hash_map>
#include <map>
#include <string>
#include <vector>
#include "Params.h"
#import "Box.h"

struct ImpossibleRelations;

struct TestCollection {
	int size();
	bool evaluateCenter(int index, Box& box);
	int evaluateBox(int index, Box& box);
	const char* getName(int index);
	int add(std::string word);
	void load(const char* fileName);
	void loadImpossibleRelations(const char* fileName);
private:
	std::map<std::string, int> stringIndex;
	std::vector<std::string> indexString;
	
	GL2ACJ evaluate1(std::string word, Params<AComplex1Jet>& params);
	bool validIdentity(std::string word, int xLattice, int yLattice, Params<AComplex1Jet>& params);
	bool evaluate(std::string word, Params<Complex>& params);
	int evaluate(std::string word, Params<AComplex1Jet>& params, bool notIdentity=false);
	void enumerate(const char* w);
	void enumerateTails(std::string s, int pCount, int lCount);
	ImpossibleRelations *impossible;
};

