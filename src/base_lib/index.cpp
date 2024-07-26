#include <vector>
#include <base-lib.h>

using namespace baselib;
using namespace std;

Index::Index() : x(0), y(0) { }
Index::Index(int x, int y) : x(x), y(y) { }

vector<Index> Index::neighbours(bool sides = true, bool corners = true)
{
	vector<Index> result;
	if (sides) {
		result.push_back(Index(x, y + 1));
		result.push_back(Index(x + 1, y));
		result.push_back(Index(x, y - 1));
		result.push_back(Index(x - 1, y));
	}
	if (corners) {
		result.push_back(Index(x + 1, y + 1));
		result.push_back(Index(x + 1, y - 1));
		result.push_back(Index(x - 1, y + 1));
		result.push_back(Index(x - 1, y - 1));
	}
	return result;
}

ostream& baselib::operator<<(std::ostream& os, const Index& index)
{
	os << "(" << index.x << ", " << index.y << ")";
	return os;
}