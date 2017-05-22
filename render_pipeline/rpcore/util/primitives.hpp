#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.h>

namespace rpcore {

RENDER_PIPELINE_DECL NodePath create_points(const std::string& name, int count);
RENDER_PIPELINE_DECL NodePath create_cube(const std::string& name);
RENDER_PIPELINE_DECL NodePath create_sphere(const std::string& name, unsigned int latitude, unsigned int longitude);

}
