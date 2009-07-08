#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
bool g_recursive = false;
bool g_verbose = false;
char* g_treeLocation;

FILE* openBox(char* boxcode)
{
	FILE* fp;
	char fileName[1000];
	char fileboxcode[1000];
	strcpy(fileboxcode, boxcode);
	if (!*fileboxcode)
		strcpy(fileboxcode, "root");
	sprintf(fileName, "%s/%s.out", g_treeLocation, fileboxcode);
	struct stat sb;
	if (0 == stat(fileName, &sb)) {
		if (g_verbose) fprintf(stderr, "opening %s\n", fileName);
		fp = fopen(fileName, "r");
		return fp;
	}
	sprintf(fileName, "%s/%s.out.gz", g_treeLocation, fileboxcode);
	if (0 == stat(fileName, &sb)) {
		char commandBuf[1000];
		if (g_verbose) fprintf(stderr, "opening %s\n", fileName);
		sprintf(commandBuf, "gzcat %s", fileName);
		fp = popen(commandBuf, "r");
		return fp;
	}
	return 0;
}

void processTree(FILE* fp, bool print, char* boxcode)
{
	int boxdepth = strlen(boxcode);
	char buf[10000];
	int depth = 0;
	while (fgets(buf, sizeof(buf), fp)) {
		bool filledHole = false;
		if (g_recursive && print && buf[0] == 'H') {
			boxcode[boxdepth + depth] = '\0';
			FILE* fpH = openBox(boxcode);
			if (fpH) {
				processTree(fpH, print, boxcode);
				fclose(fpH);
				filledHole = true;
			} else {
				fprintf(stderr, "missing %s\n", boxcode);
			}
		}
		if (print && !filledHole)
			fputs(buf, stdout);
		if (buf[0] == 'X') {
			boxcode[boxdepth + depth] = '0';
			++depth;
		} else {
			for (; depth > 0 && boxcode[boxdepth + depth-1] == '1'; --depth) {
			}
			if (depth > 0) {
				boxcode[boxdepth + depth-1] = '1';
			} else {
				boxcode[boxdepth] = '\0';
				return;
			}
		}
	}
	boxcode[boxdepth + depth] = '\0';
	fprintf(stderr, "premature EOF at %s\n", boxcode);
}
		
int main(int argc, char** argv)
{
	if (argc > 1 && !strcmp(argv[1], "-v")) {
		g_verbose = true;
		++argv;
		--argc;
	}
	if (argc > 1 && !strcmp(argv[1], "-r")) {
		g_recursive = true;
		++argv;
		--argc;
	}
	if (argc != 3) {
		fprintf(stderr, "Usage: treecat treeLocation boxcode\n");
		exit(1);
	}

	char* boxcode = argv[2];
	g_treeLocation = argv[1];
	char fileboxcode[1000];
	strcpy(fileboxcode, boxcode);
	FILE* fp = 0;
	int fileBoxLength;
	for (fileBoxLength = strlen(boxcode); fileBoxLength >= 0; --fileBoxLength) {
		fileboxcode[fileBoxLength] = '\0';
		fp = openBox(fileboxcode);
		if (fp)
			break;
	}
	boxcode += fileBoxLength;

	char fullboxcode[1000];
	strcpy(fullboxcode, fileboxcode);
	strcat(fullboxcode, boxcode);
	char buf[10000];
	while (*boxcode && fgets(buf, sizeof(buf), fp)) {
		if (buf[0] != 'X') {
			*boxcode = '\0';
			fprintf(stderr, "box = %s\n", argv[2]);
			fputs(buf, stdout);
			exit(0);
		}
		if (*boxcode == '1')
			processTree(fp, false, boxcode);
		++boxcode;
	}
	processTree(fp, true, fullboxcode);
}
