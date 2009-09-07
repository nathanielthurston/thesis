/*
 *  BallSearch.cpp
 *  horoballs
 *
 *  Created by Nathaniel Thurston on 23/04/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "BallSearch.h"
#include "CanonicalName.h"
#include "GL2C.h"
#include "Params.h"
#include <list>
#include <map>
#include <queue>
using namespace std;

namespace BallSearchImpl {
	struct Word {
		string name;
		string nameClass;
		GL2C matrix;
	};
	
	struct LatticePoint {
		Params<Complex>* params;
		int x;
		int y;
		double distance;
	};
	
	struct LowerDistance : public binary_function<const LatticePoint&, const LatticePoint&, bool>
	{
		bool operator() (const LatticePoint& a, const LatticePoint& b)
		{
			return a.distance < b.distance;
		}
	};

	int gPower(Word w)
	{
		int gCount = 0;
		for (string::size_type pos = 0; pos < w.name.size(); ++pos)
			if (w.name[pos] == 'g' || w.name[pos] == 'G')
				++gCount;
		return gCount;
	}

	
	Word operator * (const Word& lhs, const LatticePoint& rhs);
	Word operator * (const Word& lhs, const Word& rhs);
	
	struct WordPair {
		WordPair(Word* first, Word* second, Params<Complex>* params);
		void setCurrentIndex(int i);
		
		Word* firstWord;
		Word* secondWord;
		vector<LatticePoint> distances;
		int distanceIndex;
		Word currentCombination;
		double currentBallSize;
	};
	
	struct SmallerBallSize : public binary_function<const WordPair&, const WordPair&, bool>
	{
		bool operator() (const WordPair& a, const WordPair& b)
		{
			return a.currentBallSize < b.currentBallSize;
		}
	};
	
	struct BallSearch {
		BallSearch(Params<Complex> params_) :params(params_), G(constructG(params)), g(~G), m_foundBigBall(false) {}
		~BallSearch() {}
		void pushWord(Word word);
		void pushWord(string word);
		Word findWord();
		bool foundBigBall() { return m_foundBigBall; }
		void findNames(Word& word);
		void addRelator(string word);
		
		Params<Complex> params;
		CanonicalName canonicalName;
		GL2C G;
		GL2C g;
		list<Word> words;
		priority_queue<WordPair, vector<WordPair>, SmallerBallSize> pairs;
		bool m_foundBigBall;
	};
	
	void BallSearch::findNames(Word& word)
	{
		word.name = canonicalName.getCanonicalName(word.name);
		string::size_type firstG = word.name.find_first_of("gG");
		string::size_type lastG = word.name.find_last_of("gG");
		if (firstG == string::npos) {
			word.nameClass = "";
		} else {
			word.nameClass = word.name.substr(firstG, lastG - firstG + 1);
		}
	}
	
	Word inverse(Word& word)
	{
		Word inv;
		inv.name = word.name;
		reverse(inv.name.begin(), inv.name.end());
		string::size_type pos;
		for (pos = 0; pos < inv.name.size(); ++pos) {
			char c = inv.name[pos];
			if (c >= 'a' && c <= 'z')
				inv.name[pos] += 'A' - 'a';
			else
				inv.name[pos] -= 'A' - 'a';
		}
		inv.matrix = ~word.matrix;
		return inv;
	}
	
	Word operator * (const Word& lhs, const LatticePoint& rhs)
	{
		Word result;
		result.name = lhs.name;
		result.nameClass = lhs.nameClass;
		for (int i = 0; i <  rhs.x; ++i) result.name += "M";
		for (int i = 0; i < -rhs.x; ++i) result.name += "m";
		for (int i = 0; i <  rhs.y; ++i) result.name += "N";
		for (int i = 0; i < -rhs.y; ++i) result.name += "n";
		result.matrix = lhs.matrix * constructT(*rhs.params, rhs.x, rhs.y);
		return result;
	}
	
	Word operator * (const Word& lhs, const Word& rhs)
	{
		Word result;
		result.name = lhs.name + rhs.name;
		result.matrix = lhs.matrix * rhs.matrix;
		return result;
	}

	WordPair::WordPair(Word* first, Word* second, Params<Complex>* params)
		:firstWord(first), secondWord(second), distanceIndex(0)
	{
		Complex centerDiff = first->matrix.a / first->matrix.c
		 - second->matrix.a / second->matrix.c;
		int y0 = int(floor(centerDiff.imag() / params->lattice.imag()));
		int x0 = int(floor(centerDiff.real() - y0*params->lattice.real()));
		for (int x = -2-x0; x <= 3-x0; ++x) {
			for (int y = -1-y0; y <= 2-y0; ++y) {
				if (x == 0 && y == 0 && firstWord->name[0] == secondWord->name[0])
					continue;
				LatticePoint l;
				l.params = params;
				l.x = -x;
				l.y = -y;
				Complex t = centerDiff + double(x) + double(y)*params->lattice;
				l.distance = norm(t);
				if (abs(l.x) < 4 && abs(l.y < 4))
					distances.push_back(l);
			}
		}
		sort(distances.begin(), distances.end(), LowerDistance());
		setCurrentIndex(0);
	}
	
	void WordPair::setCurrentIndex(int i)
	{
		distanceIndex = i;
		if (i >= distances.size()) {
			currentBallSize = 0;
		} else {
			currentCombination = (inverse(*firstWord) * distances[i]) * (*secondWord);
			currentBallSize = 0.5 / norm( currentCombination.matrix.c);
		}
//		printf("WordPair(%s,%s):setCurrentIndex(%d) : %d,%d %f %s size=%f\n",
//			firstWord->name.c_str(), secondWord->name.c_str(),
//			i, distances[i].x, distances[i].y, distances[i].distance,
//			currentCombination.name.c_str(), currentBallSize);
	}
	
	void BallSearch::pushWord(Word word)
	{
		list<Word>::iterator it;
		// first, check to see if this word has already been pushed
		findNames(word);
		if (word.nameClass == "")
			return;
		for (it = words.begin(); it != words.end(); ++it)
			if (it->nameClass == word.nameClass)
				return;
		//printf("pushWord(%s) class=%s\n", word.name.c_str(), word.nameClass.c_str());
		words.push_back(word);
		Word* wp = &words.back();
		for (it = words.begin(); it != words.end(); ++it) {
			WordPair pair(&*it, wp, &params);
//			printf("pushWordPair(%s,%s)\n", pair.firstWord->name.c_str(), pair.secondWord->name.c_str());
			if (pair.distances.size() > 0)
				pairs.push(pair);
		}
	}
	
	void BallSearch::pushWord(string word)
	{
		Word w;
		w.name = word;
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
						w.matrix = w.matrix*constructT(params, x, y);
						x=y=0;
					}
					if (c == 'g') {
						w.matrix = w.matrix*g;
					} else if (c == 'G') {
						w.matrix = w.matrix*G;
					}
				}
			}
		}
		//printf("pushWord(%s): distance=%f\n", w.name.c_str(), 0.5 / norm(w.matrix.c));
		pushWord(w);
		Word wInv = inverse(w);
		findNames(wInv);
		pushWord(wInv);
		//fprintf(stderr, "pushed words %s(%s) and %s(%s)\n",
			//w.name.c_str(), w.nameClass.c_str(),
			//wInv.name.c_str(), wInv.nameClass.c_str());
	}
	
	Word BallSearch::findWord()
	{
		for (;;) {
			WordPair largest(pairs.top());
			Word result(largest.currentCombination);
			string oldName = result.name;
			findNames(result);			
			pairs.pop();
			largest.setCurrentIndex(largest.distanceIndex+1);
			pairs.push(largest);
			list<Word>::iterator it;
			for (it = words.begin(); it != words.end(); ++it)
				if (result.nameClass == it->nameClass)
					break;
			if (result.nameClass != "" && it == words.end()) {
				double sizeRatio = norm(result.matrix.c) / norm(words.front().matrix.c);
				//fprintf(stderr, "%s: sizeRatio = %f gPower = %d\n", result.name.c_str(), sizeRatio, gPower(result));
				if (sizeRatio < 1) {
					m_foundBigBall = true;
				}
				return result;
			}
			//fprintf(stderr, "rejecting %s^-1 *(%d,%d) %s = %s -> %s (%s)\n",
				//largest.firstWord->name.c_str(),
				//largest.distances[largest.distanceIndex-1].x,
				//largest.distances[largest.distanceIndex-1].y,
				//largest.secondWord->name.c_str(),
				//oldName.c_str(),
				//result.name.c_str(),
				//result.nameClass.c_str()
			//);
			//printf("duplicate; resorting\n");
		}
	}
	
	void BallSearch::addRelator(string w)
	{
		canonicalName.addRelator(w);
	}
}

vector<string> findWords(Params<Complex> center, vector<string> seedWords, int numWords, int maxLength,
	vector<string> relators)
{
	BallSearchImpl::BallSearch search(center);
	for (int i = 0; i < relators.size(); ++i)
		if (relators[i].size() > 0)
			search.addRelator(relators[i]);
	for (int i = 0; i < seedWords.size(); ++i)
		if (seedWords[i].size() > 0)
			search.pushWord(seedWords[i]);
	search.pushWord("g");
	search.pushWord("G");
	vector<string> foundWords;
	while (numWords > int(foundWords.size()) || (-numWords > int(foundWords.size()) && !search.foundBigBall())) {
		BallSearchImpl::Word w = search.findWord();
		if (gPower(w) > maxLength) {
			search.m_foundBigBall = false;
			continue;
		}
		//fprintf(stderr, "adding %s\n", w.name.c_str());
		search.pushWord(w);
		foundWords.push_back(w.nameClass);
	}
	return foundWords;
}
