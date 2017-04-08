#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.h>

namespace rpcore {

RPCPP_DECL NodePath create_cube(const std::string& name);
RPCPP_DECL NodePath create_sphere(const std::string& name, unsigned int latitude, unsigned int longitude);

}
