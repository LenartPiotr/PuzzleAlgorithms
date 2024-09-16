#include <base-lib.h>
#include <signpost.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
using namespace algorithms::signpost;

Algorithm::Algorithm() { }
Algorithm::~Algorithm() { }

string Algorithm::getName() {
	return "signpost";
}

void Algorithm::printFormat(ostream& o) {
	o << "Signpost file format\n";
	o << "m lines with n field descriptions\n";
	o << "Each field description is separated by a space from the others.\nIt consists of a description of the rltbe (right / left / top / bottom / end) arrows and their combinations,\nand an optional number\n";
	o << "Example:\n\n";
	o << string() +
		"b1 b b b bl\n" +
		"r rt r t l\n" +
		"br b l l t17\n" +
		"tr t tr r b\n" +
		"rt rt rt t e25\n\n";
}

void Algorithm::prepare(std::ifstream& in) {
	string line;
	vector<string> lines;
	while (getline(in, line)) {
		if (!line.empty()) lines.push_back(line);
	}

	int height = lines.size();
	if (height == 0) throw baselib::WrongFileFormatException();
	int width = 1;
	for (auto it = lines[0].begin(); it != lines[0].end(); it++) {
		if (*it == ' ') width++;
	}

	int lastIndex = width * height - 1;

	for (int y = 0; y < height; y++) {
		line = lines[y];
		auto it = line.begin();
		for (int x = 0; x < width; x++) {
			Node node;
			node.order = 0;
			node.nextNode = -1;
			node.prevNode = -1;
			Index direction;
			while (it != line.end() && *it != ' ' && *it != '\n') {
				if (*it == 'l') direction.x = -1;
				else if (*it == 'r') direction.x = 1;
				else if (*it == 't') direction.y = -1;
				else if (*it == 'b') direction.y = 1;
				else if (*it >= '0' && *it <= '9') {
					node.order *= 10;
					node.order += *it - '0';
				}
				it++;
			}
			while (it != line.end() && *it == ' ') it++;
			node.order--;
			if (node.order == 1) startIndex = y * width + x;
			else if (node.order == lastIndex) endIndex = y * width + x;
			
			if (direction != Index(0, 0)) {
				Index hook(x, y);
				hook = hook + direction;
				while (hook.x >= 0 && hook.x < width && hook.y >= 0 && hook.y < height) {
					node.next.push_back(hook.y * width + hook.x);
					hook = hook + direction;
				}
			}
			nodes.push_back(node);
		}
	}

	for (int i = 0; i < nodes.size(); i++) {
		for (int c : nodes[i].next) {
			nodes[c].prev.push_back(i);
		}
	}
}

void Algorithm::processFile(ifstream& inFile, ofstream& outFile) {
	prepare(inFile);

	simplifyGraph();

	printNodes(cout);
}

void Algorithm::cleanUp() { }

void Algorithm::disconnect(int prev, int next)
{
	auto& nextVec = nodes[prev].next;
	auto& prevVec = nodes[next].prev;
	nextVec.erase(std::remove(nextVec.begin(), nextVec.end(), next), nextVec.end());
	prevVec.erase(std::remove(prevVec.begin(), prevVec.end(), prev), prevVec.end());
}

void Algorithm::connect(int prev, int next)
{
	// setup nodes
	
	nodes[prev].nextNode = next;
	nodes[next].prevNode = prev;

	// Removes all other connections

	auto& nextVec = nodes[prev].next;
	auto& prevVec = nodes[next].prev;

	for (int other : nextVec) {
		if (other == next) continue;
		auto& othVec = nodes[other].prev;
		othVec.erase(std::remove(othVec.begin(), othVec.end(), prev), othVec.end());
	}

	for (int other : prevVec) {
		if (other == prev) continue;
		auto& othVec = nodes[other].next;
		othVec.erase(std::remove(othVec.begin(), othVec.end(), next), othVec.end());
	}

	nextVec.clear();
	nextVec.push_back(next);

	prevVec.clear();
	prevVec.push_back(prev);
	
	// Complete the order

	if (nodes[prev].order == -1 && nodes[next].order != -1) {
		int idx = next;
		int order = nodes[next].order;

		while (nodes[idx].prevNode != -1) {
			idx = nodes[idx].prevNode;
			nodes[idx].order = --order;
		}
	}

	if (nodes[prev].order != -1 && nodes[next].order == -1) {
		int idx = prev;
		int order = nodes[prev].order;

		while (nodes[idx].nextNode != -1) {
			idx = nodes[idx].nextNode;
			nodes[idx].order = ++order;
		}
	}

	// Remomve connections from extreme points to each other

	int forwardIdx = next;
	while (nodes[forwardIdx].nextNode != -1) forwardIdx = nodes[forwardIdx].nextNode;

	int backwardIdx = prev;
	while (nodes[backwardIdx].prevNode != -1) backwardIdx = nodes[backwardIdx].prevNode;

	disconnect(forwardIdx, backwardIdx);
}

void Algorithm::simplifyGraph()
{
	bool change;
	do {
		change = false;
		for (int i = 0; i < nodes.size(); i++) {
			Node& node = nodes[i];

			if (node.prevNode == -1 && node.prev.size() == 1) {
				int prev = node.prev[0];
				connect(prev, i);
				change = true;
			}

			if (node.nextNode == -1 && node.next.size() == 1) {
				int next = node.next[0];
				connect(i, next);
				change = true;
			}
		}
	} while (change);
}

void Algorithm::findPaths()
{
	//
}

void Algorithm::printNodes(std::ostream& os)
{
	for (int i = 0; i < nodes.size(); i++) {
		os << i << ": " << nodes[i].order << " - next:";
		for (int c : nodes[i].next) {
			os << " " << c;
		}
		os << "  | prev:";
		for (int c : nodes[i].prev) {
			os << " " << c;
		}
		os << endl;
	}
}

ostream& algorithms::signpost::operator<<(std::ostream& os, const Algorithm& a)
{
	//

	return os;
}