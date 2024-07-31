#include <slitherlink.h>

#include <iostream>
#include <vector>
#include <set>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>

using namespace algorithms::slitherlink;
using namespace std;

ColorBoard::ColorBoard(int width, int height) : Board(width, height), colorMargin(1), nextColor(2) {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            tab[x][y] = 0;
        }
}

void ColorBoard::colorSame(vector<Index> indexes, vector<Index> oposite = vector<Index>())
{
    set<int> group;
    for (const Index& idx : indexes) {
        int color = (*this)[idx];
        group.insert(color);
    }
    for (const Index& idx : oposite) {
        int color = (*this)[idx];
        group.insert(-color);
    }
    group.erase(0);
    int targetColor;

    if (group.empty()) {
        targetColor = nextColor++;
    }
    else if (group.size() == 1) {
        targetColor = *group.begin();
    }
    else {
        targetColor = mergeColors(group);
    }

    for (const Index& idx : indexes) {
        (*this)[idx] = targetColor;
    }
    for (const Index& idx : oposite) {
        (*this)[idx] = -targetColor;
    }
}

int ColorBoard::mergeColors(const set<int>& colors)
{
    int minimumColor = numeric_limits<int>::max();
    for (auto it = colors.begin(); it != colors.end(); it++) {
        if ((abs(*it)) < abs(minimumColor)) minimumColor = *it;
        else break;
    }

    set<int> c(colors);
    c.erase(minimumColor);

    for (int x = 0; x < getWidth(); x++) {
        for (int y = 0; y < getHeight(); y++) {
            int& fieldValue = tab[x][y];
            if (c.contains(fieldValue)) fieldValue = minimumColor;
            else if (c.contains(-fieldValue)) fieldValue = -minimumColor;
        }
    }

    return minimumColor;
}

int& ColorBoard::operator[](const Index& idx)
{
	if (inRange(idx)) {
		return tab[idx.x][idx.y];
	}
	return colorMargin;
}

const int& ColorBoard::operator[](const Index& idx) const
{
	if (inRange(idx)) {
		return tab[idx.x][idx.y];
	}
    return colorMargin;
}

ostream& algorithms::slitherlink::operator<<(ostream& os, const ColorBoard& board)
{
    int width = board.getWidth();
    int height = board.getHeight();
    vector<size_t> colWidths(width, 0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int value = board.tab[x][y];
            size_t numDigits = (value == 0) ? 2 : to_string(abs(value)).length() + 1;
            colWidths[x] = max(colWidths[x], numDigits);
        }
    }
    
    for (int y = 0; y < height; ++y) {
        os << "| ";
        for (int x = 0; x < width; ++x) {
            int value = board.tab[x][y];
            string sign = (value > 0) ? "+" : "";
            if (value == 0) {
                os << std::string(colWidths[x], '-');
            }
            else {
                os << setw(colWidths[x]) << (sign + to_string(value));
            }
            os << " ";
        }
        os << "|" << endl;
    }

    return os;
}

Algorithm::Algorithm() { }
Algorithm::~Algorithm() { }
std::string Algorithm::getName() { return "slitherlink"; }

void Algorithm::printFormat(ostream& o)
{
	o << "Slitherlink file format\n";
	o << "n x m table with characters: 0123-\n\n";
	o << "Sample table:\n\n";
	o << "21-2\n---2\n-3-1\n3-22\n\n";
}

void Algorithm::processFile(ifstream& inFile, ofstream& outFile, const string& fileName)
{
    string line;
    while (std::getline(inFile, line)) {
        outFile << fileName << ": " << line << std::endl;
    }
    colorBoard = new ColorBoard(3, 3);
}

void Algorithm::cleanUp()
{
    delete colorBoard;
}
