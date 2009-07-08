/*
 *  ImpossibleRelations.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 13/10/2007.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "ImpossibleRelations.h"
#include <map>

using namespace std;

namespace ImpossibleRelationsImpl {
	
	struct Impl : public ImpossibleRelations {
		bool isAlwaysImpossible(string word, list<string>& mandatoryIdentities);
		bool isImpossible(string word, int mCoeff, int nCoeff, list<string>& mandatoryIdentities);
		void load(const char* filePath);
	private:
		struct PossiblePower {
			bool subWordIdentityAllowed;
			bool matchRequired;
			int matchingMCoeff;
			int matchingNCoeff;
			int power;
			string subWord;
		};
		typedef multimap<string, PossiblePower> PossibleStore;
		PossibleStore possibleStore;
	};
	
	bool Impl::isAlwaysImpossible(string word, list<string>& mandatoryIdentities)
	{
		PossibleStore::iterator it = possibleStore.lower_bound(word);
		list<string> mandatory;
		while (it != possibleStore.end() && it->first == word) {
			PossiblePower possible = it->second;
			if (!possible.matchRequired) {
				if (!possible.subWordIdentityAllowed) {
					return true;
				} else {
					mandatory.push_back(possible.subWord);
				}
			}
			++it;
		}
		mandatoryIdentities.swap(mandatory);
		return false;
	}
	
	bool Impl::isImpossible(string word, int mCoeff, int nCoeff, list<string>& mandatoryIdentities)
	{
		PossibleStore::iterator it = possibleStore.lower_bound(word);
		list<string> mandatory;
		while (it != possibleStore.end() && it->first == word) {
			PossiblePower possible = it->second;
//			printf("considering %s/%s\n", word.c_str(), possible.subWord.c_str());
			if (!possible.matchRequired || (mCoeff == possible.matchingMCoeff && nCoeff == possible.matchingNCoeff)) {
				if (!possible.subWordIdentityAllowed) {
					return true;
				} else {
					if (!possible.matchRequired) {
						int diffX = possible.matchingMCoeff - mCoeff;
						int diffY = possible.matchingNCoeff - nCoeff;
						if (diffX % possible.power != 0 || diffY % possible.power != 0) {
							return true;
						}
					}
					mandatory.push_back(possible.subWord);
				}
			}
			++it;
		}
		mandatoryIdentities.swap(mandatory);
		return false;
	}
	
	// PossiblePower gMggMgMgg 0 0 -1 0 MgMgg^2 ~ 0 n=3 base=3 {PossiblePower MgMgg 0 1 0 0 Mg^3 ~ 2 n=1 base=1 }

	void Impl::load(const char* filePath)
	{
		char buf[1000];
		char wordBuf[1000];
		char subWordBuf[1000];
		int subWordIdentityAllowed;
		int matchRequired;
		PossiblePower possible;
		FILE* fp = fopen(filePath, "r");
		while (fp && fgets(buf, sizeof(buf), fp)) {
			int n = sscanf(buf, "PossiblePower %s %d %d %d %d %[gGmMnN]^%d",
				wordBuf, &subWordIdentityAllowed, &matchRequired,
				&possible.matchingMCoeff, &possible.matchingNCoeff,
				subWordBuf, &possible.power);
			if (n == 7) {
				possible.subWordIdentityAllowed = subWordIdentityAllowed;
				possible.matchRequired = matchRequired;
				possible.subWord = subWordBuf;
				possibleStore.insert(make_pair(string(wordBuf), possible));
			} else {
				if (n > 0) fprintf(stderr, "incomplete line %s", buf);
				return;
			}
		}
	}
}

ImpossibleRelations* ImpossibleRelations::create(const char* relationsFilePath)
{
	ImpossibleRelationsImpl::Impl* impossible = new ImpossibleRelationsImpl::Impl();
	impossible->load(relationsFilePath);
	return impossible;
}
