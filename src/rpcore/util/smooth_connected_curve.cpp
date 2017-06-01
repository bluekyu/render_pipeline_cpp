#include "smooth_connected_curve.h"

#include <boost/format.hpp>

#include <curveFitter.h>

namespace rpcore {

SmoothConnectedCurve::SmoothConnectedCurve(void)
{
	_cv_points = std::vector<LVecBase2f>(
		{ LVecBase2f(0, 0), LVecBase2f(0.5, 0), LVecBase2f(1.0, 0) }
	);
	build_curve();
}

void SmoothConnectedCurve::build_curve(void)
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

std::string SmoothConnectedCurve::serialize(void) const
{
	boost::format point_formats("[{%5.10f},{%5.10f}]");

	std::string result;
	result.reserve(_cv_points.size() * 40);
	result += "[";
	
	for (const auto& point: _cv_points)
	{
		result += (point_formats % (point[0]) % (point[1])).str();
		result += ",";
	}
	
	if (_cv_points.size() > 0)
		result.pop_back();
	result += "]";

	return result;
}

}
