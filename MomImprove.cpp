/*
 *  MomImprove.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 08/10/2007.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
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

struct NamedBox : public Box {
	NamedBox() {}
	NamedBox(Box box) :Box(box) {}
	
	string name;
	NamedBox child(int dir) const;
};

NamedBox NamedBox::child(int dir) const
{
	NamedBox child(Box::child(dir));
	child.name = name;
	child.name.append(1, '0'+dir);
	return child;
}

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

struct Improvement {
	Improvement(int i) :index(i) {}
	int index;
	vector<bool> testResults;
};


int main(int argc, char** argv)
{
	vector<NamedBox> boxStack;
	vector<Improvement> improvements;
	vector<NamedBox> position;
	NamedBox currentBox;
	
	boxStack.push_back(currentBox);
	char buf[10000];
	for (int i = 1; i < argc; ++i) {
		if (isdigit(argv[i][0])) {
			improvements.push_back(atoi(argv[i]));
		} else {
			improvements.push_back(tests.add(argv[i]));
		}
	}
	while (gets(buf)) {
//		fprintf(stderr, "visiting %s (%s)\n", currentBox.name.c_str(), buf);
		int depth = currentBox.name.length();
		if (depth >= position.size())
			position.push_back(currentBox);
		else
			position[depth] = currentBox;
		for (int i = 0; i < improvements.size(); ++i) {
			Improvement& imp = improvements[i];
			while (imp.testResults.size() >= depth && imp.testResults.size() > 0)
				imp.testResults.pop_back();
		}
		if (!strcmp(buf, "X")) {
			int foundImprovement = -1;
			for (int i = 0; i < improvements.size(); ++i) {
				Improvement& imp = improvements[i];
				while (6 + imp.testResults.size() <= depth
				 || imp.testResults.size() <= depth &&
				  (imp.testResults.empty() || imp.testResults.back())) {
					bool result = tests.evaluateCenter(imp.index,
						position[imp.testResults.size()]);
//					fprintf(stderr, "evaluate %s @ %s = %s\n",
//						tests.getName(imp.index),
//						position[imp.testResults.size()].name.c_str(),
//						result ? "true" : "false");
					imp.testResults.push_back(result);
				}
				if (imp.testResults.back()) {
//					fprintf(stderr, "testing %s @ %s\n",
//						tests.getName(imp.index),
//						position[depth].name.c_str());
					bool result = tests.evaluateBox(imp.index, position[depth]);
					if (result) {
						fprintf(stderr, "improved %s @ %s\n",
							tests.getName(imp.index),
							position[depth].name.c_str());
						foundImprovement = i;
						break;
					}
				}
			}
			if (foundImprovement >= 0) {
				skip();
				currentBox = boxStack.back();
				boxStack.pop_back();
				printf("%s\n", tests.getName(improvements[foundImprovement].index));
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
				bool result = true; //tests.evaluateBox(testNumber, currentBox);
				if (!result) {
					fprintf(stderr, "FAILED %d %s\n", testNumber, currentBox.name.c_str());
				}
				currentBox = boxStack.back();
				boxStack.pop_back();
				printf("%s\n", buf);
			}
		}
	}
}
