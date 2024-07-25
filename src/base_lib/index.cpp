#include <base-lib.h>

using namespace baselib;
using namespace std;

Index::Index() : x(0), y(0) { }
Index::Index(int x, int y) : x(x), y(y) { }

Index* baselib::Index::neighbours()
{
	// TODO
	return nullptr;
}

ostream& baselib::operator<<(std::ostream& os, const Index& index)
{
	os << "(" << index.x << ", " << index.y << ")";
	return os;
}