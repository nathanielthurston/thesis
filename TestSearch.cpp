/*
 *  TestSearch.cpp
 *  mom
 *
 *  Created by Nathaniel Thurston on 27/09/2007.
 *  Copyright 2007 THingith ehf.. All rights reserved.
 *
 */

#include "TestSearch.h"
#include "GL2C.h"
#include <vector>
#include <queue>
#include <string>

using namespace std;

extern int g_maxWordLength;
void print(GL2C& m, const char* what) {
	Complex center = m.a / m.c;
	Complex height = 1.0 /  m.c;
	printf("%10s%9g+%9gI\t%9g+%9gI\n%10s%9g+%9gI\t%9g+%9gI\n",
		what, m.a.real(), m.a.imag(), m.b.real(), m.b.imag(),
		"", m.c.real(), m.c.imag(), m.d.real(), m.d.imag());
	printf("%10s%10f @ %10f + %10f I\n",
		"horoball", abs(height), center.real(), center.imag());
}

struct LatticePoint {
	LatticePoint(const Params<Complex>& params, int n_, int x_, int y_) :n(n_), x(x_), y(y_) {
		center = Complex(x+y*params.lattice.real(), y*params.lattice.imag());
		dist = abs(center); 
	}
	int n;
	int x;
	int y;
	Complex center;
	double dist;
};

bool operator < (const LatticePoint&  a, const LatticePoint&  b)
{
	return a.dist < b.dist;
}

struct Lattice {
	Lattice(const Params<Complex>& params) { initPoints(params); }
	void initPoints(const Params<Complex>& params);
	int maxN;
	vector<LatticePoint> points;
	LatticePoint& operator[] (int n);
};

LatticePoint& Lattice::operator[](int n)
{
	assert(n < maxN);
	return points[n];
}

void Lattice::initPoints(const Params<Complex>& params)
{
	double maxDist = 50;
	int maxX = int(floor(maxDist));
	int maxY = int(floor(maxDist / params.lattice.imag()));
	for (int y = -maxY; y <= maxY; ++y) {
		int offset = int(floor(-params.lattice.real()*y));
		for (int x = -maxX-offset; x <= maxX-offset+1; ++x) {
			points.push_back(LatticePoint(params, 0, x, y));
		}
	}
	sort(points.begin(), points.end());
	for (maxN = 0; maxN < points.size(); ++maxN) {
		points[maxN].n = maxN;
		if (points[maxN].dist > maxDist)
			break;
	}
	points[maxN].n = -1;
}

double distance(GL2C& m, const Params<Complex>& params)
{
	Complex sc = m.c / params.loxodromicSqrt;
	Complex center = m.a / sc;
	Complex heightInv = sc;
//	printf("distance = %f %f %f %f\n", center.real(), center.imag(), heightInv.real(), heightInv.imag());
	return center.real()*center.real()
	 + center.imag()*center.imag()
	 + heightInv.real()*heightInv.real()
	 + heightInv.imag()*heightInv.imag();
}



struct QueueEntry {
	QueueEntry(LatticePoint& latticePoint_) :latticePoint(latticePoint_) {}
	string baseWord;
	GL2C baseMatrix;
	int baseX;
	int baseY;
	LatticePoint latticePoint;
	GL2C matrix;
	double distance;
	int sequence;
};

bool operator < (const QueueEntry& a, const QueueEntry& b) {
	return a.distance > b.distance;
}
typedef priority_queue<QueueEntry> WordQueue;


string translation(int x, int y)
{
	string translationWord;
	for (int i = 0; i < x; ++i)
		translationWord += "M";
	for (int i = 0; i < -x; ++i)
		translationWord += "m";
	for (int i = 0; i < y; ++i)
		translationWord += "N";
	for (int i = 0; i < -y; ++i)
		translationWord += "n";
	return translationWord;
}

void normalize(QueueEntry& e, const Params<Complex>& params)
{
	GL2C& m = e.baseMatrix;
	if (abs(m.c) < 0.7)
		return;
	Complex center = m.a / m.c;
	double y = center.imag() / params.lattice.imag();
	e.baseY = -int(rint(y));
	double x = center.real() + e.baseY*params.lattice.real();
	if (fabs(x) > 5 || fabs(y) > 5)
		printf("big translation %f,%f\n", x, y);
	e.baseX = -int(rint(x));
	e.matrix = constructT(params, e.baseX, e.baseY) * e.baseMatrix;
	e.baseMatrix = e.matrix;
//	double priorDist = e.distance;
	e.distance = distance(e.matrix, params);
//	printf("normalize before=%f after=%f\n", priorDist, e.distance);
}

void pushQueue(WordQueue& queue, QueueEntry& e)
{
	static int seq = 0;
	if (e.baseWord.length() + abs(e.latticePoint.x) + abs(e.latticePoint.y) > 1.3 * g_maxWordLength)
		return;
	string s = translation(e.baseX + e.latticePoint.x, e.baseY + e.latticePoint.y) + e.baseWord;
	e.sequence = ++seq;
//	printf("%s(%d) distance=%f\n", s.c_str(), e.sequence, e.distance);
//	print(e.matrix, "matrix");
	queue.push(e);
}

void initQueue(WordQueue& queue, Lattice& lattice, const Params<Complex>& params, GL2C& G, GL2C& g)
{
	QueueEntry e(lattice[0]);
	e.baseWord = "G";
	e.matrix = e.baseMatrix = G;
	e.baseX = e.baseY = 0;
	e.distance = distance(e.matrix, params);
	pushQueue(queue, e);
	e.baseWord = "g";
	e.matrix = e.baseMatrix = g;
	e.distance = distance(e.matrix, params);
	pushQueue(queue, e);
}

string processNextWord(WordQueue& queue, Lattice& lattice, const Params<Complex>& params, GL2C& G, GL2C& g)
{
	if (queue.empty())
		return "";
	string result;
	const QueueEntry e = queue.top();
	queue.pop();
	string s = translation(e.baseX + e.latticePoint.x, e.baseY + e.latticePoint.y) + e.baseWord;
//	printf("processing %s(%d) distance = %f\n", s.c_str(), e.sequence, e.distance);
	QueueEntry el(e);
	el.latticePoint = lattice[e.latticePoint.n+1];
	el.matrix = constructT(params, el.latticePoint.x, el.latticePoint.y) * e.baseMatrix;
	el.distance = distance(el.matrix, params);
	pushQueue(queue, el);
	if (s[0] != 'g') {
		QueueEntry Ge(lattice[0]);
		Ge.baseWord = "G";
		Ge.baseWord += s;
		Ge.baseMatrix = G*e.matrix;
		if (abs(Ge.baseMatrix.c) > 0.9*abs(g.c) && abs(Ge.baseMatrix.c) > 0.9) {
			normalize(Ge, params);
			pushQueue(queue, Ge);
		} else {
//			print(Ge.baseMatrix, "identity");
			result = Ge.baseWord;;
		}
	}
	if (s[0] != 'G') {
		QueueEntry ge(lattice[0]);
		ge.baseWord = "g";
		ge.baseWord += s;
		ge.baseMatrix = g*e.matrix;
		if (abs(ge.baseMatrix.c) > 0.9*abs(g.c) && abs(ge.baseMatrix.c) > 0.9) {
			normalize(ge, params);
			pushQueue(queue, ge);
		} else {
//			print(ge.baseMatrix, "identity");
			result = ge.baseWord;
		}
	}
	return result;
}

string findWord(const Params<Complex>& center)
{
	GL2C G(constructG(center));
	GL2C g(~G);
	WordQueue wordQueue;
	Lattice lattice(center);
	initQueue(wordQueue, lattice, center, G, g);
	for (int i = 0; i < 10000; ++i) {
		string found = processNextWord(wordQueue, lattice, center, G, g);
		if (!found.empty())
			return found;
	}
	return "";
}
