/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <luse.h>
#include <parametricCurveCollection.h>

namespace rpcore {

/**
 * Interface to a curve which also manages connecting the end of the
 * curve with the beginning.
 */
class SmoothConnectedCurve
{
public:
    SmoothConnectedCurve();

    /** Returns a list of all controll points. */
    const std::vector<LVecBase2>& get_control_points() const;

    /** Sets the cv points to the given list of points. */
    void set_control_points(const std::vector<LVecBase2>& points);

    /** Returns the display color of the curve. */
    const LVecBase3& get_color() const;

    /** Sets the display color of the curve. */
    void set_color(const LVecBase3& rgb);

    /** Sets the curve to be linear, and only use a single value. */
    void set_single_value(PN_stdfloat val);

    /** Appends a new cv and returns the index of the attached cv. */
    size_t append_cv(PN_stdfloat x, PN_stdfloat y);

    /**
     * Attempts to remove the cv at the given index, does nothing if only
     * one control point is left.
     */
    void remove_cv(size_t index);

    /** Rebuilds the curve based on the controll point values. */
    void build_curve();

    /** Updates the cv point at the given index. */
    void set_cv_value(size_t index, PN_stdfloat x_value, PN_stdfloat y_value);

    /**
     * Returns the value on the curve ranging whereas the offset should be
     * from 0 to 1 (0 denotes the start of the curve). The returned value will
     * be a value from 0 to 1 as well.
     */
    PN_stdfloat get_value(PN_stdfloat offset) const;

    /** Returns the value of the curve as yaml list. */
    std::string serialize() const;

private:
    bool _modified = false;
    int _border_points = 1;
    LVecBase3 _color = LVecBase3(0, 0, 0);
    std::vector<LVecBase2> _cv_points;
    PT(ParametricCurveCollection) _curve;
};

// ************************************************************************************************
inline const std::vector<LVecBase2>& SmoothConnectedCurve::get_control_points() const
{
    return _cv_points;
}

inline void SmoothConnectedCurve::set_control_points(const std::vector<LVecBase2>& points)
{
    _cv_points = points;
    _modified = true;
    build_curve();
}

inline const LVecBase3& SmoothConnectedCurve::get_color() const
{
    return _color;
}

inline void SmoothConnectedCurve::set_color(const LVecBase3& rgb)
{
    _color = rgb;
}

inline void SmoothConnectedCurve::set_single_value(PN_stdfloat val)
{
    _modified = false;
    _cv_points = { {0.5f, val} };
    build_curve();
}

inline size_t SmoothConnectedCurve::append_cv(PN_stdfloat x, PN_stdfloat y)
{
    _cv_points.push_back({x, y});
    build_curve();
    _modified = true;
    return _cv_points.size() - 1;
}

inline void SmoothConnectedCurve::remove_cv(size_t index)
{
    if (_cv_points.size() > 1)
        _cv_points.erase(_cv_points.begin() + index);
    _modified = true;
    build_curve();
}

inline void SmoothConnectedCurve::set_cv_value(size_t index, PN_stdfloat x_value, PN_stdfloat y_value)
{
    _cv_points[index] = LVecBase2(x_value, y_value);
    _modified = true;
}

inline PN_stdfloat SmoothConnectedCurve::get_value(PN_stdfloat offset) const
{
    LVecBase3 point(0);
    _curve->evaluate_xyz(offset, point);
    const PN_stdfloat y = point.get_y();
    return (std::max)(0.0f, (std::min)(1.0f, y));
}

}
