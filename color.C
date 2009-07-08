
#include "GL2C.h"
#include <vector>
#include <string>
#include <queue>
#import "Params.h"
#include "TestSearch.h"
#import "Box.h"
#include "TestCollection.h"
#include <stdio.h>

using namespace std;
TestCollection tests;
int nodeCount = 0;

vector<string> holes;

struct ParamNode {
	ParamNode() :lChild(0), rChild(0), isOpen(true), contradiction(-1) {}
	ParamNode(ParamNode& parent, int dir);
	string name;
	Box box;
	ParamNode* lChild;
	ParamNode* rChild;
	
	bool isOpen;
	int contradiction;
	vector<int> nonContradictions;
	
	void descendOneLevel(int depthRemaining);
	void testLeafNodes(vector<char> testState, int depthRemaining);
	void searchForNewTests(int depthRemaining);
	void removeHoles();
};


ParamNode::ParamNode(ParamNode& parent, int dir)
	:name(parent.name),
	box(parent.box.child(dir)),
	lChild(0),
	rChild(0),
	isOpen(true),
	contradiction(-1)
{
	name.append(1, '0' + dir);
	printf("creating %s\n", name.c_str());
}

// increase the depth of the tree, forking all open leaf nodes.
// remove all non-contradictions which are too shallow to still be relevant.
void ParamNode::descendOneLevel(int depthRemaining)
{
	if (!isOpen)
		return;
	if (lChild) {
		lChild->descendOneLevel(depthRemaining-1);
		rChild->descendOneLevel(depthRemaining-1);
	} else {
		lChild = new ParamNode(*this, 0);
		rChild = new ParamNode(*this, 1);
		nodeCount += 2;
	}
}

// test all open leaf nodes against possible tests
void ParamNode::testLeafNodes(vector<char> testState, int depthRemaining)
{
	if (!isOpen)
		return;
	vector<int>::iterator it;
	int i;
	vector<int> testsToTry;
	if (depthRemaining >= 7) {
		for (it = nonContradictions.begin(); it != nonContradictions.end(); ++it)
			testState[*it] = 2; // re-test these ones; they are obsolete
	} else {
		for (it = nonContradictions.begin(); it != nonContradictions.end(); ++it)
			testState[*it] = 0; // don't test these ones again: we're not deep enough
		for (i = 0; i < testState.size(); ++i) {
			if (testState[i] > 0 && (depthRemaining == 0 || testState[i] == 2)) {
				bool validCenter = tests.evaluateCenter(i, box);
//				printf("EVALUATE %d@%s = %s [%d]\n", i, name.c_str(), validCenter ? "true" : "false", depthRemaining);
				if (!validCenter) {
					nonContradictions.push_back(i);
					testState[i] = 0;
				} else if (depthRemaining == 0) {
					testsToTry.push_back(i);
				}
			}
		}
	}
	if (lChild) {
		lChild->testLeafNodes(testState, depthRemaining-1);
		rChild->testLeafNodes(testState, depthRemaining-1);
		if (!(lChild->isOpen || rChild->isOpen)) {
			isOpen = false;
			nonContradictions.clear();
		}
	} else {
		int elimCount = 0;
		for (vector<int>::iterator it = testsToTry.begin(); it != testsToTry.end(); ++it) {
//			printf("TRYING %d@%s\n", *it, name.c_str());
			if (tests.evaluateBox(*it, box)) {
				printf("ELIMINATED %d@%s\n", *it, name.c_str());
				contradiction = *it;
				isOpen = false;
				++elimCount;
				break;
			}
		}
		double low, high;
		box.volumeRange(low, high);
		printf("BOX %s %d %d %f-%f\n", name.c_str(), testsToTry.size(), elimCount, low, high);
	}
}

// search for new tests
void ParamNode::searchForNewTests(int depthRemaining)
{
	if (!isOpen)
		return;
	if (!lChild || depthRemaining <= 0) {
		Params<Complex> center(box.center());
		string w = findWord(center);
		if (!w.empty()) {
			tests.add(w);
		}
	} else {
		lChild->searchForNewTests(depthRemaining-1);
		rChild->searchForNewTests(depthRemaining-1);
	}
}

void ParamNode::removeHoles()
{
	if (!isOpen)
		return;
	if (lChild) {
		lChild->removeHoles();
		rChild->removeHoles();
	} else {
		for (int i = 0; i < holes.size(); ++i) {
			if (name == holes[i])
				isOpen = false;
		}
	}
}

void loadHoles(const char* holesName)
{
	char buf[10000];
	FILE *fp = fopen(holesName, "r");
	while (fp && fgets(buf, sizeof(buf), fp)) {
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
		holes.push_back(buf);
	}
}

int main(int argc, char** argv)
{	
	Params<Complex> whitehead;
	whitehead.lattice = Complex(0, 2);
	whitehead.loxodromicSqrt = Complex(0, sqrt(2));
	whitehead.parabolic = Complex(0, 1);
	
	printf("word = %s\n", findWord(whitehead).c_str());
	
	tests.load("goodWords");
	tests.loadImpossibleRelations("wordPowers.out");
	ParamNode rootNode;

	if (argc > 1) {
		for (const char* c = argv[1]; *c; ++c) {
			if (*c == '0' || *c == '1') {
				if (*c == '0')
					rootNode.box = rootNode.box.child(0);
				else
					rootNode.box = rootNode.box.child(1);
				rootNode.name.append(1, *c);
			}
		}
	}
	if (argc > 2) {
		loadHoles(argv[2]);
	}
	for (int depth = 1; depth < 50; ++depth) {
		if (nodeCount > 5000000)
			break;
		rootNode.descendOneLevel(depth);
		rootNode.removeHoles();
		if (depth + rootNode.name.length() > 12)
			rootNode.searchForNewTests(depth-6);
		rootNode.testLeafNodes(vector<char>(tests.size(), 1), depth);
	}
	return 0;
}
