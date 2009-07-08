#include <stdio.h>
#include <map>
#include <string>
#include <vector>
using namespace std;

vector<string> indexWord;
map<string, int> wordIndex;

struct TreeNode {
	TreeNode() :testIndex(-1), left(0), right(0) {}
	int testIndex;
	TreeNode* left;
	TreeNode* right;
};

typedef map<string, string> NodeMap;
NodeMap files;

TreeNode loadFile(const char* name, const char* basePos)
{
	int elimCount = 0;
	map<int, int> indexMap;
	TreeNode root;
	TreeNode* node;
	FILE *fp;
	char buf[10000];
	if (strstr(name, ".gz")) {
		sprintf(buf, "gzcat %s", name);
		fp = popen(buf, "r");
	} else {
		fp = fopen(name, "r");
	}
	if (!fp) {
		fprintf(stderr, "Can't open '%s'\n", name);
	}
	int myIndex;
	char myWord[10000];
	char pos[10000];
	while (fp && fgets(buf, sizeof(buf), fp)) {
		if (buf[strlen(buf)-1] != '\n')
			break;
		if (2 == sscanf(buf, "adding %d=%s", &myIndex, myWord)) {
			map<string,int>::iterator it = wordIndex.find(myWord);
			int globalIndex = 0;
			if (it != wordIndex.end()) {
				globalIndex = it->second;
			} else {
				globalIndex = indexWord.size();
				wordIndex[myWord] = indexWord.size();
				indexWord.push_back(myWord);
			}
			indexMap[myIndex+7] = globalIndex;
		} else if (2 == sscanf(buf, "ELIMINATED %d@%s", &myIndex, pos)) {
			if (strncmp(pos, basePos, strlen(basePos))) {
				fprintf(stderr, "unexpected box '%s' not inside '%s'\n", pos, basePos);
				exit(1);
			}

			node = &root;
			for (char*p = pos + strlen(basePos); *p; ++p) {
				if (*p == '0')  {
					if (!node->left)
						node->left = new TreeNode();
					node = node->left;
				} else {
					if (!node->right)
						node->right = new TreeNode();
					node = node->right;
				}
			}
			if (myIndex < 7)
				node->testIndex = myIndex;
			else
				node->testIndex = indexMap[myIndex];
			++elimCount;
		}
	}
	fprintf(stderr, "read %d nodes for %s from %s\n", elimCount, basePos, name);
	if (strstr(name, ".gz"))
		pclose(fp);
	else
		fclose(fp);
	return root;
}

void deleteTree(TreeNode* pos) {
	if (pos) {
		deleteTree(pos->left);
		deleteTree(pos->right);
		delete pos;
	}
}

NodeMap::iterator printFiles(TreeNode *pos, char* name, NodeMap::iterator it)
{
	if (it != files.end() && it->first == name) {
		deleteTree(pos);
		pos = new TreeNode(loadFile(it->second.c_str(), it->first.c_str()));
		++it;
	}
	if (!pos) {
		printf("HOLE\n");
		fprintf(stderr, "HOLE %s\n", name);
	} else {
		if (pos->testIndex > 6) {
			printf("%s\n", indexWord[pos->testIndex].c_str());
		} else if (pos->testIndex >= 0) {
			printf("%d\n", pos->testIndex);
		} else {
			printf("X\n");
			int l = strlen(name);
			name[l+1] = '\0';
			name[l] = '0';
			it = printFiles(pos->left, name, it);
			name[l] = '1';
			it = printFiles(pos->right, name, it);
			name[l] = '0';
			name[l] = '\0';
		}
	}
	if (pos) delete pos;
	return it;
}

int main(int argc, char** argv) {
	indexWord.resize(7);
	char buf[1000];
	for (int i = 1; i < argc; ++i) {
		char* fileName = argv[i];
		char* s = fileName;
		char* w = buf;
		while (*s) {
			if (*s == '0' || *s == '1')
				*w++ = *s;
			++s;
		}
		*w = '\0';

		files[buf] = fileName;
	}
	buf[0] = '\0';
	TreeNode *root = new TreeNode();
	printFiles(root, buf, files.begin());
}
