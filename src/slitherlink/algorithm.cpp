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

ColorBoard::ColorBoard(int width, int height) : Board(width, height), colorMargin(1), nextColor(2), updateFlag(false) {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            tab[x][y] = 0;
        }
}

void ColorBoard::colorSame(vector<Index> indexes, vector<Index> oposite = vector<Index>())
{
    set<int> group;
    bool anyChange = false;
    for (const Index& idx : indexes) {
        int color = (*this)[idx];
        group.insert(color);
    }
    for (const Index& idx : oposite) {
        int color = (*this)[idx];
        group.insert(-color);
    }
    const auto it = group.find(0);
    if (it != group.end()) {
        group.erase(it);
        anyChange = true;
    }
    int targetColor;

    if (group.empty()) {
        targetColor = nextColor++;
    }
    else if (group.size() == 1) {
        targetColor = *group.begin();
    }
    else {
        targetColor = mergeColors(group);
        anyChange = true;
    }

    if (!anyChange) return;

    updateFlag = true;
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
    cout << "File: " << fileName << endl;
    string line;
    vector<string> lines;
    while (std::getline(inFile, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    if (lines.size() == 0) {
        throw WrongFileFormatException();
    }
    
    int width = lines[0].size();
    int height = lines.size();
    
    numbers = new Board<int>(width, height);
    colorBoard = new ColorBoard(width, height);

    char c;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            c = lines[y][x];
            (*numbers)[Index(x, y)] = c >= '0' && c <= '3' ? c - '0' : -1;
        }
    }

    findPatterns();
    mainLoop();

    outFile << *colorBoard << endl;
}

void Algorithm::cleanUp()
{
    delete colorBoard;
}

void Algorithm::findPatterns()
{
    int w = numbers->getWidth();
    int h = numbers->getHeight();

    // find 3 : 3 pattern in diagonal
    vector<IndexTransform> transforms{ IndexTransform(2, 2), IndexTransform(2, 2, true) };
    for (const auto& t : transforms) {
        for (int x = 0; x < w - 1; x++) {
            for (int y = 0; y < h - 1; y++) {
                Index hook(x, y);
                Index idx_num3_1 = Index(0, 0) * t + hook;
                Index idx_num3_2 = Index(1, 1) * t + hook;
                if ((*numbers)[idx_num3_1] != 3 || (*numbers)[idx_num3_2] != 3) continue;
                Index idx_ne1_1 = Index(-1, 0) * t + hook;
                Index idx_ne1_2 = Index(0, -1) * t + hook;
                Index idx_ne2_1 = Index(2, 1) * t + hook;
                Index idx_ne2_2 = Index(1, 2) * t + hook;
                colorBoard->colorSame({ idx_num3_1 }, { idx_ne1_1, idx_ne1_2 });
                colorBoard->colorSame({ idx_num3_2 }, { idx_ne2_1, idx_ne2_2 });
            }
        }
    }

    // find 3 : 3 pattern in row
    vector<IndexTransform> transforms2{ IndexTransform(2, 1), IndexTransform(2, 1, false, false, 1) };
    for (const auto& t : transforms2) {
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                Index hook(x, y);
                Index idx_num3_1 = Index(0, 0) * t + hook;
                Index idx_num3_2 = Index(1, 0) * t + hook;
                try {
                    if ((*numbers)[idx_num3_1] != 3 || (*numbers)[idx_num3_2] != 3) continue;
                }
                catch (const exception& e) {
                    continue;
                }
                Index idx_num3_1b = Index(-1, 0) * t + hook;
                Index idx_num3_2b = Index(2, 0) * t + hook;
                Index idx_up1 = Index(0, -1) * t + hook;
                Index idx_up2 = Index(1, -1) * t + hook;
                Index idx_down1 = Index(0, 1) * t + hook;
                Index idx_down2 = Index(1, 1) * t + hook;
                colorBoard->colorSame({ idx_num3_1, idx_num3_2b }, { idx_num3_2, idx_num3_1b });
                colorBoard->colorSame({ idx_up1, idx_up2 });
                colorBoard->colorSame({ idx_down1, idx_down2 });
            }
        }
    }

    // find 0
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            if ((*numbers)[Index(x, y)] != 0) continue;
            auto indexes = Index(x, y).neighbours(true, false);
            indexes.push_back(Index(x, y));
            colorBoard->colorSame(indexes);
        }
    }
}

void Algorithm::mainLoop()
{
    do {
        colorBoard->updateFlag = false;

        stepCountNeighbours();
        stepCheckCrosses();

    } while (colorBoard->updateFlag);
}

void Algorithm::stepCountNeighbours()
{
    int w = colorBoard->getWidth();
    int h = colorBoard->getHeight();
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Index hook(x, y);
            int num = (*numbers)[hook];
            if (num == -1) continue;
            int color = (*colorBoard)[hook];

            vector<Index> neighours = hook.neighbours(true, false);
            vector<int> colors;
            for (const Index& idx : neighours) colors.push_back((*colorBoard)[idx]);
            
            // Full known
            if (color != 0) {
                int sameColorsCount = 0;
                int revColorsCount = 0;
                for (const int& col : colors) {
                    if (col == color) sameColorsCount++;
                    else if (col == -color) revColorsCount++;
                }

                if (sameColorsCount + revColorsCount != 4) {
                    if (sameColorsCount == 4 - num) {
                        vector<Index> rest;
                        for (int i = 0; i < 4; i++) {
                            if (colors[i] != color) rest.push_back(neighours[i]);
                        }
                        colorBoard->colorSame({ hook }, rest);
                        continue;
                    }

                    if (revColorsCount == num) {
                        vector<Index> rest;
                        for (int i = 0; i < 4; i++) {
                            if (-colors[i] != color) rest.push_back(neighours[i]);
                        }
                        rest.push_back(hook);
                        colorBoard->colorSame(rest);
                        continue;
                    }
                }
            }
        }
    }
}

void Algorithm::stepCheckCrosses()
{
    int w = colorBoard->getWidth();
    int h = colorBoard->getHeight();
    IndexTransform t(2, 2);
    for (int rotation = 0; rotation < 4; rotation++) {
        t.rotation = rotation;
        for (int x = 0; x < w - 1; x++) {
            for (int y = 0; y < h - 1; y++) {
                Index hook(x, y);
                Index i_lt = Index(0, 0) * t + hook;
                Index i_lb = Index(0, 1) * t + hook;
                Index i_rt = Index(1, 0) * t + hook;
                Index i_rb = Index(1, 1) * t + hook;
                int lt = (*colorBoard)[i_lt];
                int rt = (*colorBoard)[i_rt];
                int lb = (*colorBoard)[i_lb];
                int rb = (*colorBoard)[i_rb];
                
                // cross with the same color
                if (lb == -rb && rb == -rt && lt != lb && rb != 0) {
                    colorBoard->colorSame({ i_lt, i_lb });
                }

                // cross with different colors
                if (lt == -rt && lb == -rb && lt != 0 && lb != 0 && lt != lb) {
                    colorBoard->colorSame({ i_lt, i_lb });
                }
            }
        }
    }
}
