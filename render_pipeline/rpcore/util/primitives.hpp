#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.hpp>

namespace rpcore {

RENDER_PIPELINE_DECL NodePath create_points(const std::string& name, const std::vector<LPoint3f>& positions,
    GeomEnums::UsageHint buffer_hint=Geom::UsageHint::UH_static);

/**
 * Create 2D plane primitive.
 *
 * left-bottom is (-0.5, 0, -0.5) and right-top is (0.5, 0, 0.5).
 */
RENDER_PIPELINE_DECL NodePath create_plane(const std::string& name);

/**
 * Create cube (box) primitive.
 *
 * min is (-0.5, -0.5, -0.5) and max is (0.5, 0.5, 0.5).
 */
RENDER_PIPELINE_DECL NodePath create_cube(const std::string& name);

/**
 * Create sphere primitiv.e
 *
 * Center is (0, 0, 0) and radius 1.0.
 */
RENDER_PIPELINE_DECL NodePath create_sphere(const std::string& name, unsigned int latitude, unsigned int longitude);

}
