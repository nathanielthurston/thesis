/*
 *  MomCheck.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 01/10/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */


#include "GL2C.h"
#include <vector>
#include <string>
#include <queue>
#import "Params.h"
#include "TestSearch.h"
#import "Box.h"
#include <ctype.h>
#include "TestCollection.h"
using namespace std;
TestCollection tests;


void skip()
{
	int n = 1;
	int numSkipped = 0;
	char buf[10000];
	while (n >= 0 && gets(buf)) {
		if (!strcmp(buf, "X"))
			++n;
		else
			--n;
		++numSkipped;
	}
	fprintf(stderr, "SKIPPED %d\n", numSkipped);
}

int main(int argc, char** argv)
{
	vector<NamedBox> boxStack;
	NamedBox currentBox;
	boxStack.push_back(currentBox);
	char buf[10000];
	int improveNumber = -1;
	if (argc > 1) {
		if (isdigit(argv[1][0])) {
			improveNumber = atoi(argv[1]);
		} else {
			improveNumber = tests.add(argv[1]);
		}
	}
	while (gets(buf)) {
		if (!strcmp(buf, "X")) {
			if (improveNumber >= 0 && tests.evaluateBox(improveNumber, currentBox)) {
				skip();
				currentBox = boxStack.back();
				boxStack.pop_back();
				printf("%s\n", tests.getName(improveNumber));
			} else {
				boxStack.push_back(currentBox.child(1));
				currentBox = currentBox.child(0);
				printf("X\n");
			}
		} else {
			if (!strcmp(buf, "HOLE")) {
				fprintf(stderr, "HOLE %s\n", currentBox.name.c_str());
			} else {
				int testNumber = 0;
				if (isdigit(*buf)) {
					testNumber = atoi(buf);
				} else {
					testNumber = tests.add(buf);
				}
				int result = tests.evaluateBox(testNumber, currentBox);
				if (!result) {
					fprintf(stderr, "FAILED %d %s\n", testNumber, currentBox.name.c_str());
				} else if (result == 2) {
					fprintf(stderr, "ID %s %s\n", tests.getName(testNumber),
						currentBox.name.c_str());
				}
				currentBox = boxStack.back();
				boxStack.pop_back();
				printf("%s\n", buf);
			}
		}
	}
}
