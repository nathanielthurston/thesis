#ifndef __QUASI_RELATORS_H
#define __QUASI_RELATORS_H

#include <string>
#include <map>
#include <vector>

class QuasiRelators {
public:
	std::string getName(std::string w);               // get the canonical name of a quasi-relator
	void addQuasiRelator(std::string w);        // record that this word is a quasi-relator

	std::vector<std::string> allWords();
	std::vector<std::string> wordClasses();
	std::string desc();                               // string describing this set of quasi-relators
	bool isQuasiRelator(std::string w);         // is this word a quasi-relator?

private:
	typedef std::map< std::string, std::string > NameStore;
	NameStore names;
	std::vector<std::string> nameVector;
	std::string inverse(std::string w);
};

#endif
