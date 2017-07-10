#pragma once

#include <cmath>

namespace rplibs {

/** @return The result of x % y in Python. */
inline float py_fmod(float x, float y)
{
    return std::fmod(std::fmod(x, y) + y, y);
}

inline double py_fmod(double x, double y)
{
    return std::fmod(std::fmod(x, y) + y, y);
}

inline long double py_fmod(long double x, long double y)
{
    return std::fmod(std::fmod(x, y) + y, y);
}

}
