#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.h>

namespace rpcore {

/** Creates a rgb color from a given string. */
LVecBase3 rgb_from_string(const std::string& text, float min_brightness=0.6f);

/**
 * 'Snaps' a shadow map to make sure it always is on full texel centers.
 * This ensures no flickering occurs while moving the shadow map.
 * This works by projecting the Point (0,0,0) to light space, compute the
 * texcoord differences and offset the light world space position by that.
 */
RENDER_PIPELINE_DECL void snap_shadow_map(const LMatrix4f& mvp, NodePath cam_node, int resolution);

}   // namespace rpcore
