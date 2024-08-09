#include <base-lib.h>

using namespace baselib;

baselib::IndexTransform::IndexTransform(int width, int height, bool flipX, bool flipY, int rotation)
    : areaWidth(width), areaHeight(height), flipX(flipX), flipY(flipY), rotation(rotation) { }

Index baselib::IndexTransform::transform(const Index& idx) const
{
    int x = idx.x;
    int y = idx.y;
    int w = areaWidth - 1;
    int h = areaHeight - 1;
    int t;
    if (flipX) x = w - x;
    if (flipY) y = h - y;
    switch (rotation) {
    case 1:
        t = w - x;
        x = y;
        y = t;
        break;
    case 2:
        y = h - y;
        x = w - x;
        break;
    case 3:
        t = x;
        x = h - y;
        y = t;
    }
    return Index(x, y);
}

Index baselib::IndexTransform::operator*(const Index& idx)
{
    return transform(idx);
}

Index baselib::operator*(const Index& idx, const IndexTransform& transform)
{
    return transform.transform(idx);
}