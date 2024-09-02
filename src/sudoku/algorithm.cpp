#include <sudoku.h>

#include <iostream>
#include <string>
#include <stack>

using namespace algorithms::sudoku;
using namespace std;

Algorithm::Algorithm() { }
Algorithm::~Algorithm() { }

string Algorithm::getName() {
	return "sudoku";
}

void Algorithm::printFormat(ostream& o) {
	o << "Sudoku file format\n";
	o << "9 lines with 9 characters: 0-9 and space or '-' for empty\n";
	o << "Example:\n\n";
	o << string() +
		"   6   75\n" +
		"98     2 \n" +
		"  2  1 4 \n" +
		"  6  4   \n" +
		"74       \n" +
		"  1 8 7  \n" +
		"47       \n" +
		"3   2 1 9\n" +
		"       6 \n\n";
}

void Algorithm::processFile(ifstream& inFile, ofstream& outFile) {
	prepare(inFile);
	
	if (mainLoop()) {
		outFile << *this;
	}
	else {
		outFile << "No solutions\n";
	}
}

void Algorithm::cleanUp() { }

void Algorithm::prepare(ifstream& inFile)
{
	for (int i = 0; i < 9; i++) {
		rows[i] = 0x1ff;
		cols[i] = 0x1ff;
		area[i % 3][i / 3] = 0x1ff;
	}

	string line;
	int y = 0;
	int v, mask;

	while (std::getline(inFile, line)) {
		if (line.empty()) continue;
		if (line.size() != 9) continue;

		for (int x = 0; x < 9; x++) {
			if (line[x] == ' ' || line[x] == '-') {
				tab[x][y] = -1;
				taken[x][y] = false;
			}
			else {
				v = line[x] - '0' - 1;
				mask = 0x1ff - (1 << v);
				tab[x][y] = v;
				taken[x][y] = true;
				cols[x] &= mask;
				rows[y] &= mask;
				area[x / 3][y / 3] &= mask;
			}
		}

		y++;
	}

	if (y != 9) {
		throw WrongFileFormatException();
	}
}

inline void Algorithm::nextFreeField(int& i) const {
	do i++;
	while (i < 81 && taken[i % 9][i / 9]);
}

inline void Algorithm::backToPreviousField(int& i) const {
	do i--;
	while (i >= 0 && taken[i % 9][i / 9]);
}

bool Algorithm::mainLoop() {
	int l, r_mask;
	
	int i = 0;
	while (taken[i % 9][i / 9]) i++;
	
	stack<Memory> s;

	// Index i loop through all fields. If index end in 81 that mean puzzle is solved. If end in 0 that mean no solution found
	while (i < 81 && i >= 0) {
		int x = i % 9;
		int y = i / 9;

		// If field value is -1 index come here from previous field. Otherwise index returned from next field because it could not find a solution.
		if (tab[x][y] == -1) {
			
			// Remember masks before any modifications
			Memory m;
			m.row = rows[y];
			m.col = cols[x];
			m.area = area[x / 3][y / 3];
			m.mask = m.row & m.col & m.area;

			// Mask eq 0 mean that no number can be entered - back to previous field
			if (m.mask == 0) {
				backToPreviousField(i);
				continue;
			}

			// Find first possible digit to enter
			l = 0;
			while (!((1 << l) & m.mask)) l++;
			
			// Update table and masks
			tab[x][y] = l;
			r_mask = 0x1ff - (1 << l);
			cols[x] &= r_mask;
			rows[y] &= r_mask;
			area[x / 3][y / 3] &= r_mask;

			s.push(m);
			nextFreeField(i);
		}
		else {
			Memory m = s.top();

			// Find next possible digit to enter
			l = tab[x][y] + 1;
			while (l < 9 && !((1 << l) & m.mask)) l++;

			if (l >= 9) {
				// No possible digit - back to previous field
				// Remind the masks
				cols[x] = m.col;
				rows[y] = m.row;
				area[x / 3][y / 3] = m.area;

				tab[x][y] = -1;
				s.pop();
				backToPreviousField(i);
				continue;
			}

			tab[x][y] = l;
			r_mask = 0x1ff - (1 << l);
			cols[x] = m.col & r_mask;
			rows[y] = m.row & r_mask;
			area[x / 3][y / 3] = m.area & r_mask;
			nextFreeField(i);
		}
	}

	return i >= 81;
}

ostream& algorithms::sudoku::operator<<(ostream& o, const Algorithm& a)
{
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			o << a.tab[x][y] + 1;
		}
		o << endl;
	}
	return o;
}