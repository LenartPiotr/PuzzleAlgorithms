#include <base-lib.h>
#include <signpost.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

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

	height = lines.size();
	if (height == 0) throw baselib::WrongFileFormatException();
	width = 1;
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
	prepareRanges();

	mainLoop();

	outFile << (*this);
}

void Algorithm::cleanUp() {
	nodes.clear();
	ranges.clear();
}

void Algorithm::mainLoop() {
	do {
		do {
			updateFlag = false;
			simplifyGraph();
			connectRanges();
		} while (!updateFlag);
		findPaths();
	} while (!updateFlag);
}

void Algorithm::disconnect(int prev, int next)
{
	updateFlag = true;

	auto& nextVec = nodes[prev].next;
	auto& prevVec = nodes[next].prev;
	nextVec.erase(std::remove(nextVec.begin(), nextVec.end(), next), nextVec.end());
	prevVec.erase(std::remove(prevVec.begin(), prevVec.end(), prev), prevVec.end());
}

void Algorithm::connect(int prev, int next)
{
	if (nodes[prev].nextNode != -1) return;

	updateFlag = true;

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

	int rangeStart = prev;
	int rangeEnd = next;

	if (nodes[prev].order == -1 && nodes[next].order != -1) {
		int idx = next;
		int order = nodes[next].order;

		while (nodes[idx].prevNode != -1) {
			idx = nodes[idx].prevNode;
			nodes[idx].order = --order;
		}

		rangeStart = idx;
	}

	if (nodes[prev].order != -1 && nodes[next].order == -1) {
		int idx = prev;
		int order = nodes[prev].order;

		while (nodes[idx].nextNode != -1) {
			idx = nodes[idx].nextNode;
			nodes[idx].order = ++order;
		}

		rangeEnd = idx;
	}

	// Remomve connections from extreme points to each other

	int forwardIdx = next;
	while (nodes[forwardIdx].nextNode != -1) forwardIdx = nodes[forwardIdx].nextNode;

	int backwardIdx = prev;
	while (nodes[backwardIdx].prevNode != -1) backwardIdx = nodes[backwardIdx].prevNode;

	disconnect(forwardIdx, backwardIdx);

	// Add node to ranges

	int rangeStartOrder = nodes[rangeStart].order;
	int rangeEndOrder = nodes[rangeEnd].order;

	if (rangeStartOrder != -1 && rangeEndOrder != -1) {
		
		// Try expand range with already existing node
		auto it = std::find_if(ranges.begin(), ranges.end(), [rangeStartOrder](Range r) { return r.endOrder == rangeStartOrder; });
		if (it != ranges.end()) {
			it->endOrder = rangeEndOrder;
			it->endNode = rangeEnd;
		}
		else {
			// Create new range
			for (it = ranges.begin(); it != ranges.end() && it->endOrder < rangeStartOrder; it++);
			Range newRange;
			newRange.startNode = rangeStart;
			newRange.endNode = rangeEnd;
			newRange.startOrder = rangeStartOrder;
			newRange.endOrder = rangeEndOrder;
			it = ranges.insert(it, newRange);
		}

		// Check if new range should be connected to next one
		auto nextRangeIt = it + 1;
		if (nextRangeIt != ranges.end() && nextRangeIt->startOrder == it->endOrder) {
			// Join two ranges
			it->endNode = nextRangeIt->endNode;
			it->endOrder = nextRangeIt->endOrder;
			ranges.erase(nextRangeIt);
		}
	}
}

void Algorithm::prepareRanges()
{
	auto cmp = [this](int a, int b) { return nodes[a].order < nodes[b].order; };
	set<int, decltype(cmp)> nodesWithOrder(cmp);
	
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i].order != -1) nodesWithOrder.insert(i);
	}

	Range range{ -10, -10, -10, -10 };
	
	int prevNodeIndex;
	Node* n;
	Node* prevNode;

	for (auto& i : nodesWithOrder) {
		n = &nodes[i];

		if (range.endOrder + 1 == n->order) {
			prevNode->nextNode = i;
			n->prevNode = prevNodeIndex;
			range.endOrder = n->order;
			range.endNode = i;
		}
		else {
			if (range.endNode >= 0) {
				ranges.push_back(range);
			}
			range = { n->order, i, n->order, i };
		}

		prevNodeIndex = i;
		prevNode = n;
	}
	if (range.startOrder >= 0) ranges.push_back(range);
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

void Algorithm::connectRanges()
{
	auto it = ranges.begin();
	auto next = it + 1;
	while (next != ranges.end()) {
		if (it->endOrder + 1 == next->startOrder) {
			connect(it->endNode, next->startNode);
			next = it + 1;
		}
		else {
			it++;
			next++;
		}
	}
}

void Algorithm::searchPath(vector<vector<int>>& solutions, vector<int>& currentSolution, vector<bool>& visited, int node, int order, int targetNode, int targetOrder)
{
	if (order >= targetOrder) return;
	if (order == targetOrder - 1) {
		for (int nd : nodes[node].next) {
			if (nd == targetNode) {
				solutions.push_back(currentSolution);
				break;
			}
		}
		return;
	}

	for (int nd : nodes[node].next) {
		if (visited[nd]) continue;
		if (nodes[nd].order != -1) continue;
		currentSolution.push_back(nd);
		visited[nd] = true;
		searchPath(solutions, currentSolution, visited, nd, order + 1, targetNode, targetOrder);
		currentSolution.pop_back();
		visited[nd] = false;
	}
}

void Algorithm::findPaths()
{
	auto it = ranges.begin();
	if (it == ranges.end()) return;
	auto next = it + 1;

	vector<vector<int>> solutions;
	vector<int> currentSolution;
	vector<bool> visited(nodes.size());

	visited.assign(nodes.size(), false);

	while (next != ranges.end()) {
		solutions.clear();
		currentSolution.clear();
		currentSolution.reserve(next->startOrder - it->endOrder + 1);
		currentSolution.push_back(it->endNode);
		
		std::fill(visited.begin(), visited.end(), false);

		searchPath(solutions, currentSolution, visited, it->endNode, it->endOrder, next->startNode, next->startOrder);

		if (solutions.size() != 1) {
			it++;
			next = it + 1;
			continue;
		}

		currentSolution = solutions[0];
		currentSolution.push_back(next->startNode);

		for (int i = 0; i + 1 < currentSolution.size(); i++) {
			connect(currentSolution[i], currentSolution[i + 1]);
		}

		next = it + 1;
	}
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

	printRanges(os);
}

void Algorithm::printRanges(std::ostream& os) {
	os << "\n\nRanges:";
	for (auto r : ranges) {
		os << " " << r.startOrder << "-" << r.endOrder;
	}
	os << "\n";
}

ostream& algorithms::signpost::operator<<(std::ostream& os, const Algorithm& a)
{
	for (int y = 0; y < a.height; y++) {
		for (int x = 0; x < a.width; x++) {
			if (x != 0) os << " ";
			os << a.nodes[y * a.width + x].order + 1;
		}
		os << endl;
	}

	return os;
}