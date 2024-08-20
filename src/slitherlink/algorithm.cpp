#include <slitherlink.h>

#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include <functional>

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
    // First check how many color are in passed indexes.
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

void ColorBoard::normalizeColors()
{
    set<int> colors;
    int w = getWidth();
    int h = getHeight();
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            colors.insert(abs(tab[x][y]));
        }
    int num = 2;
    auto it = colors.begin();
    auto rit = colors.rbegin();
    while (it != colors.end() && rit != colors.rend() && *it <= *rit) {
        while (it != colors.end() && *it <= *rit && *it <= num) {
            if (*it == num) num++;
            it++;
        }
        if (it != colors.end() && *it <= *rit) {
            int from = *rit;
            int to = num;
            for (int x = 0; x < w; x++)
                for (int y = 0; y < h; y++) {
                    if (tab[x][y] == from) tab[x][y] = to;
                    if (tab[x][y] == -from) tab[x][y] = -to;
                }
            *rit++;
            num++;
        }
    }
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

int* ColorBoard::operator[](const int& x)
{
    return tab[x];
}

const int* ColorBoard::operator[](const int& x) const
{
    return tab[x];
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
    
    // colorBoard->normalizeColors();
    // outFile << *colorBoard << endl;
    outFile << *this;
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

    // find 0, fill update numbers
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            int num = (*numbers)[Index(x, y)];
            if (num == -1) continue;
            if (num == 0) {
                auto indexes = Index(x, y).neighbours(true, false);
                indexes.push_back(Index(x, y));
                colorBoard->colorSame(indexes);
            }
            else {
                updateNumbers.push_back(Index(x, y));
            }
        }
    }
}

void Algorithm::mainLoop()
{
    int limit = 10000;
    do {
        do {
            colorBoard->updateFlag = false;

            stepCountNeighbours();
            stepCheckCrosses();

            if (--limit == 0) {
                cout << "LIMIT!" << endl;
                break;
            }

        } while (colorBoard->updateFlag);
        
        stepExpandAreas();
    } while (colorBoard->updateFlag);
}

void Algorithm::stepCountNeighbours()
{
    vector<Index> toRemove;
    for (const Index& hook : updateNumbers) {
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
                    toRemove.push_back(hook);
                    continue;
                }

                if (revColorsCount == num) {
                    vector<Index> rest;
                    for (int i = 0; i < 4; i++) {
                        if (-colors[i] != color) rest.push_back(neighours[i]);
                    }
                    rest.push_back(hook);
                    colorBoard->colorSame(rest);
                    toRemove.push_back(hook);
                    continue;
                }
            }
        }
        
        // Partial known by double color
        int colorDouble = 0;
        int indexColorDouble = 0;
        for (int i = 0; i < 4; i++) {
            if (colors[i] != 0) {
                for (int j = i + 1; j < 4; j++) {
                    if (colors[i] == colors[j]) {
                        colorDouble = colors[i];
                        indexColorDouble = i;
                    }
                }
            }
        }
        if (colorDouble != 0) {
            vector<Index> restIndexes;
            for (int i = 0; i < 4; i++) {
                if (colors[i] != colorDouble) restIndexes.push_back(neighours[i]);
            }
            switch (num) {
            case 1:
                colorBoard->colorSame({ hook, neighours[indexColorDouble] });
                if (restIndexes.size() == 2) colorBoard->colorSame({ restIndexes[0] }, { restIndexes[1] });
                break;
            case 2:
                colorBoard->colorSame({ neighours[indexColorDouble] }, restIndexes);
                break;
            case 3:
                colorBoard->colorSame({ hook }, { neighours[indexColorDouble] });
                if (restIndexes.size() == 2) colorBoard->colorSame({ restIndexes[0] }, { restIndexes[1] });
                break;
            }
            continue;
        }

        // Partial known by opposite color
        for (int i = 0; i < 4; i++) {
            if (colors[i] != 0) {
                for (int j = i + 1; j < 4; j++) {
                    if (colors[i] == -colors[j]) {
                        colorDouble = colors[i];
                        indexColorDouble = i;
                    }
                }
            }
        }
        if (colorDouble != 0) {
            vector<Index> restIndexes;
            for (int i = 0; i < 4; i++) {
                if (colors[i] != colorDouble && colors[i] != -colorDouble) restIndexes.push_back(neighours[i]);
            }
            switch (num) {
            case 1:
                restIndexes.push_back(hook);
                colorBoard->colorSame(restIndexes);
                break;
            case 2:
                if (restIndexes.size() == 2) colorBoard->colorSame({ restIndexes[0] }, { restIndexes[1] });
                break;
            case 3:
                colorBoard->colorSame({ hook }, { restIndexes });
                break;
            }
            continue;
        }
    }

    // Remove indexes
    for (const Index& idx : toRemove) {
        updateNumbers.erase(find(updateNumbers.begin(), updateNumbers.end(), idx));
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

void Algorithm::stepExpandAreas()
{
    BFSAreaVerifier bfs(*colorBoard);

    vector<BFSAreaResult> areas = bfs.findAll();

    int endAreasCount = 0;

    for (auto& area : areas) {
        if (area.color == -1) endAreasCount++;
        if (!area.connectOutside) {
            if (area.connections.size() == 1) colorBoard->colorSame({ *area.connections.begin(), *area.area.begin() });
            else if (area.connections.size() > 1) {
                int connectionsColor = (*colorBoard)[*area.connections.begin()];
                bool oneColor = true;
                for (const Index& i : area.connections) {
                    if ((*colorBoard)[i] != connectionsColor) {
                        oneColor = false;
                        break;
                    }
                }
                if (oneColor) {
                    colorBoard->colorSame({ *area.connections.begin(), *area.area.begin() });
                }
            }
        }
    }

    if (endAreasCount > 1) {
        for (auto& area : areas) {
            if (area.color == -1) {
                if (area.connections.size() == 1) colorBoard->colorSame({ *area.connections.begin(), *area.area.begin() });
                else if (area.connections.size() > 1) {
                    int connectionsColor = (*colorBoard)[*area.connections.begin()];
                    bool oneColor = true;
                    for (const Index& i : area.connections) {
                        if ((*colorBoard)[i] != connectionsColor) {
                            oneColor = false;
                            break;
                        }
                    }
                    if (oneColor) {
                        colorBoard->colorSame({ *area.connections.begin(), *area.area.begin() });
                    }
                }
            }
        }
    }
}

ostream& algorithms::slitherlink::operator<<(ostream& os, const Algorithm& a)
{
    string h = reinterpret_cast<const char*>(u8"\u2500");
    string v = reinterpret_cast<const char*>(u8"\u2502");
    string rb = reinterpret_cast<const char*>(u8"\u250C");
    string lb = reinterpret_cast<const char*>(u8"\u2510");
    string rt = reinterpret_cast<const char*>(u8"\u2514");
    string lt = reinterpret_cast<const char*>(u8"\u2518");

    int width = a.numbers->getWidth();
    int height = a.numbers->getHeight();

    ColorBoard& col = *a.colorBoard;
    Board<int>& num = *a.numbers;
    
    function<string(int, int)> getCorner = [&col, h, v, rb, lb, rt, lt](int x, int y) {
        int clt = col[Index(x, y)];
        if (clt == 0) return string(" ");
        int crt = col[Index(x + 1, y)];
        int clb = col[Index(x, y + 1)];
        int crb = col[Index(x + 1, y + 1)];
        if (clt == crt) {
            if (clb == crb && clb == -clt) return h;
            if (clb == -crb && clb == -clt) return lb;
            if (clb == -crb && clb == clt) return rb;
        }
        else if (clt == -crt) {
            if (clb == -crb && clb == clt) return v;
            if (clb == crb && clb == -clt) return lt;
            if (clb == crb && clb == clt) return rt;
        }
        return string(" ");
    };

    for (int y = -1; y < height; y++) {
        if (y != -1) {
            if (col[Index(-1, y)] == -col[Index(0, y)] && col[Index(-1, y)] != 0)
                os << v;
            else os << " ";
            for (int x = 0; x < width; x++) {
                if (num[Index(x, y)] != -1) os << num[Index(x, y)];
                else os << " ";
                if (col[Index(x, y)] == -col[Index(x + 1, y)] && col[Index(x, y)] != 0)
                    os << v;
                else os << " ";
            }
            os << endl;
        }
        
        os << getCorner(-1, y);
        for (int x = 0; x < width; x++) {
            if (col[Index(x, y)] == -col[Index(x, y + 1)] && col[Index(x, y)] != 0)
                os << h;
            else os << " ";
            os << getCorner(x, y);
        }
        os << endl;
    }

    return os;
}

// BFSAreaVerifier

BFSAreaVerifier::BFSAreaVerifier(const ColorBoard& colors) : visited(colors.getWidth(), colors.getHeight()), colors(colors) { }

BFSAreaResult BFSAreaVerifier::bfs(Index start)
{
    int c = colors[start];

    BFSAreaResult result;
    result.color = c;
    result.connectOutside = false;

    queue<Index> q;
    q.push(start);

    while (!q.empty()) {
        Index i = q.front();
        q.pop();
        if (!visited.inRange(i)) {
            result.connectOutside = true;
            continue;
        }
        if (colors[i] != c) {
            if (colors[i] != -c) result.connections.insert(i);
            continue;
        }
        if (visited[i]) {
            continue;
        }
        result.area.insert(i);
        for (Index& idx : i.neighbours(true, false)) {
            q.push(idx);
        }
        visited[i] = true;
    }

    return result;
}

vector<BFSAreaResult> BFSAreaVerifier::findAll()
{
    vector<BFSAreaResult> results;
    int w = colors.getWidth();
    int h = colors.getHeight();

    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
            visited[x][y] = false;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            if (!visited[x][y] && colors[x][y] != 0) results.push_back(bfs(Index(x, y)));
        }
    }

    return results;
}
