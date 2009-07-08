/*
 *  BallSearch.h
 *  horoballs
 *
 *  Created by Nathaniel Thurston on 23/04/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#import "Params.h"

#import <vector>
#import <string>

std::vector<std::string> findWords(
	Params<Complex> center,
	std::vector< std::string > seedWords,
	int numWords,
	int maxLength,
	std::vector< std::string > quasiRelators
);
