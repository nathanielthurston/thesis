#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

struct Node {
	Node(string value_ = string()) :value(value_), treeSize(1), depth(0), lChild(0), rChild(), isComplete(value != "X\n") {}
	string value;
	int treeSize;
	int depth;
	Node* lChild;
	Node* rChild;
	bool isComplete;
};

struct Options {
	string destination;
	int maxDepth;
	int maxSize;
};

void printTree(FILE* fp, Node* n, int printDepth=0)
{
	fprintf(fp, "%s", n->value.c_str());
	if (n->lChild) {
		printTree(fp, n->lChild, printDepth+1);
		printTree(fp, n->rChild, printDepth+1);
	}
}

void deleteTree(Node* n)
{
	if (n->lChild) {
		deleteTree(n->lChild);
		deleteTree(n->rChild);
	}
	delete n;
}
void truncateTree(Node* n)
{
	n->value = "HOLE\n";
	n->treeSize = 1;
	n->depth = 0;
	deleteTree(n->lChild);
	deleteTree(n->rChild);
	n->lChild = n->rChild = 0;
}
	
void printAndTruncateTree(Node* n, vector<Node>& location, string destination)
{
	destination += "/";
	string boxCode;
	for (vector<Node>::iterator it = location.begin(); it != location.end(); ++it) {
		if (it->lChild)
			boxCode += "1";
		else
			boxCode += "0";
	}
	if (boxCode.empty()) {
		destination += "root";
	} else {
		destination += boxCode;
	}
	destination += ".out";
	FILE* fp = fopen(destination.c_str(), "w");
	printTree(fp, n);
	fclose(fp);
	truncateTree(n);
}

void treetac(Options& options)
{
	char buf[10000];
	vector<Node> location;
	while (fgets(buf, sizeof(buf), stdin)) {
		location.push_back(Node(buf));
		while (location.back().isComplete) {
			Node n = location.back();
			location.pop_back();
			if (location.empty() || n.depth > options.maxDepth || n.treeSize > options.maxSize)
				printAndTruncateTree(&n, location, options.destination);
			if (location.empty())
				return;
				
			Node& p = location.back();
			p.treeSize += n.treeSize;
			if (n.depth >= p.depth)
				p.depth = n.depth+1;
			if (p.lChild) {
				p.rChild = new Node(n);
				p.isComplete = true;
			} else {
				p.lChild = new Node(n);
			}
		}
	}
	fprintf(stderr, "incomplete tree\n");
	exit(1);
}

int main(int argc, char** argv)
{
	Options options;
	if (argc != 2) {
		fprintf(stderr, "Usage: treetac destination\n");
		exit(1);
	}
	options.destination = argv[1];
	options.maxDepth = 100;
	options.maxSize = 1000000;
	treetac(options);
}
