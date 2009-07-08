/*
 *  Evaluate.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 09/10/2007.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Box.h"

#include <stdio.h>

void print(const AComplex1Jet& j, const char* what) {
	Complex c = j.center();
	Complex d0 = D(0, j);
	Complex d1 = D(1, j);
	Complex d2 = D(2, j);
	printf("%5s = %8.3f  %8.3f  %8.3f  %8.3f\n", what, c.real(), d0.real(), d1.real(), d2.real());
	printf("%5s   %8.3fI %8.3fI %8.3fI %8.3fI\n", "", c.imag(), d0.imag(), d1.imag(), d2.imag());
	printf("%5s err=%8.3g size=%8.3g abs=[%8.3g, %8.3g]\n",
		"", j.error(), j.size(), minabs(j), maxabs(j));
}

GL2ACJ evaluate(const char* wordBuf, const char* boxBuf, GL2ACJ& g)
{
	Box box;
	const char* bnp;
	for ( bnp = boxBuf; *bnp; ++bnp) {
		if (*bnp == '0') {
			box = box.child(0);
		} else if (*bnp == '1') {
			box = box.child(1);
		} else {
			printf("unexpected box character '%c'", *bnp);
			break;
		}
	}
	
	Params<AComplex1Jet> params = box.cover();
	
	GL2ACJ G(constructG(params));
	g = ~G;
	GL2ACJ M(constructT(params,1,0));
	GL2ACJ m(constructT(params,-1,0));
	GL2ACJ N(constructT(params, 0, 1));
	GL2ACJ n(constructT(params, 0, -1));
	
	AComplex1Jet zero(0.), one(1.);
	GL2ACJ w(one,zero,zero,one);
	
	for (const char* wp = wordBuf; *wp; ++wp) {
		switch(*wp) {
			case 'g': w = w*g; break;
			case 'G': w = w*G; break;
			case 'm': w = w*m; break;
			case 'M': w = w*M; break;
			case 'n': w = w*n; break;
			case 'N': w = w*N; break;
		}
	}
	return w;
}
void printResult(GL2ACJ& w, GL2ACJ& g, const char* wordBuf, const char* boxBuf)
{
		printf("%s(%s) = {\n", wordBuf, boxBuf);
		print(w.a, "A");
		print(w.b, "B");
		print(w.c, "C");
		print(w.d, "D");
		printf("}\n");
		print(w.c/g.c, "R");
		print(w.c, "R-");
		print(w.a/w.c, "ctr");
		if (maxabs(w.c/g.c) < 1) {
			print(w.b, "T");
		}
}

int main(int argc, char** argv)
{
	char lineBuf[10000];
	char boxBuf[10000];
	char wordBuf[10000], oWordBuf[10000], rotBuf[10000];
	while (gets(lineBuf)) {
		int n = sscanf(lineBuf, "%s %s %s", boxBuf, wordBuf, oWordBuf);
		if (n < 2) break;
		GL2ACJ g;
		if (argc > 1) {
			int n = strlen(wordBuf);
			int sign = 0;
			int i;
			for (i = 0; i < n; ++i) {
				strcpy(rotBuf, wordBuf+i);
				strncpy(rotBuf+(n-i), wordBuf, i);
				GL2ACJ w = evaluate(rotBuf, boxBuf, g);
				if (minabs(w.b) > 0 || minabs(w.c) > 0) {
					printf("%s %s BC %d %s\n", boxBuf, wordBuf, i, rotBuf);
					break;
				} else if (minabs(w.a-1) == 0 && minabs(w.d-1) == 0) {
					sign = 1;
				} else if (minabs(w.a+1) == 0 && minabs(w.d+1) == 0) {
					sign = -1;
				} else {
					printf("%s %s !AD %d %s\n", boxBuf, wordBuf, i, rotBuf);
					break;
				}
			}
			if (i == n) {
				printf("%c %s %s\n", sign > 0 ? '+' : '-', wordBuf, boxBuf);
			}
		} else {
			GL2ACJ w = evaluate(wordBuf, boxBuf, g);
			printResult(w, g, wordBuf, boxBuf);
			if (n == 3) {
				GL2ACJ ow = evaluate(oWordBuf, boxBuf, g);
				printResult(ow, g, wordBuf, boxBuf);
				printf("====\n");
				print(w.a/w.c - ow.a/ow.c, "diff");
			}
		}
	}
}
