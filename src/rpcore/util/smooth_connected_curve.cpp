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

#include "rpcore/util/smooth_connected_curve.hpp"

#include <fmt/format.h>

#include <curveFitter.h>

namespace rpcore {

SmoothConnectedCurve::SmoothConnectedCurve()
{
    _cv_points = std::vector<LVecBase2f>(
        { LVecBase2f(0, 0), LVecBase2f(0.5, 0), LVecBase2f(1.0, 0) }
    );
    build_curve();
}

void SmoothConnectedCurve::build_curve()
{
    auto sorted_points = _cv_points;
    std::sort(sorted_points.begin(), sorted_points.end(), [](const LVecBase2f& lhs, const LVecBase2f& rhs){
        return lhs[0] < rhs[0];
    });

    const LVecBase2f& first_point = sorted_points[0];
    CurveFitter fitter;

    // Duplicate curve at the beginning
    for (int i = 0; i < _border_points; ++i)
    {
        // TODO: what mean?
        //end_point = _cv_points[(-i + _border_points - 1) % _cv_points.size()];
        LVecBase2f end_point = first_point;
        fitter.add_xyz(0.0, LVecBase3f(0, end_point[1], 0));
    }

    // Append the actual points
    for (const auto& point: _cv_points)
    {
        // Clamp point x position to avoid artifacts at the beginning
        float point_t = max(0.01f, point[0]);
        fitter.add_xyz(point_t, LVecBase3f(point_t, point[1], 0));
    }

    // Duplicate curve at the end
    for (int i = 0; i < _border_points; ++i)
    {
        // TODO: what mean?
        //start_point = self._cv_points[i % len(self._cv_points)]
        LVecBase2f start_point = first_point;
        fitter.add_xyz(1.0f, LVecBase3f(1, start_point[1], 0));
    }

    fitter.sort_points();
    fitter.wrap_hpr();
    fitter.compute_tangents(1.0f);

    _curve = fitter.make_hermite();
}

std::string SmoothConnectedCurve::serialize() const
{
    std::string result;
    result.reserve(_cv_points.size() * 40);
    result += "[";

    for (const auto& point: _cv_points)
    {
        result += fmt::format("[{{{:5.10f}}},{{{:5.10f}}}]", point[0], point[1]);
        result += ",";
    }

    if (_cv_points.size() > 0)
        result.pop_back();
    result += "]";

    return result;
}

}
