/*
 *  ImpossibleRelations.h
 *  mom
 *
 *  Created by Nathaniel Thurston on 13/10/2007.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <list>

struct ImpossibleRelations
{
public:
	// returns true if this word is impossible regardless of which parabolic element is composed.
	// If the return value is false, sets mandatoryIdentities to the list of sub-words which must also be identities.
	virtual bool isAlwaysImpossible(std::string word, std::list<std::string>& mandatoryIdentities) = 0;
	// same as isAlwaysImpossible, but in this case the parabolic element is known.
	virtual bool isImpossible(std::string word, int mCoeff, int nCoeff, std::list<std::string>& mandatoryIdentities) = 0;
	static ImpossibleRelations* create(const char* relationsFilePath);
};