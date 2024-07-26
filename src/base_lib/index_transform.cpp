#include <base-lib.h>

using namespace baselib;

baselib::IndexTransform::IndexTransform(const Index& hook, int width, int height) : hook(hook), areaWidth(width), areaHeight(height), flipX(false), flipY(false), rotation(0) { }

Index baselib::IndexTransform::operator*(const Index& idx)
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
    return hook + Index(x, y);
}
