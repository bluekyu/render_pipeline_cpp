/**
 * This is C++ porting codes of direct/src/interval/LerpInterval.py
 */

#include "render_pipeline/rppanda/interval/lerp_interval.hpp"

namespace rppanda {

int LerpNodePathInterval::lerp_node_path_num = 1;

TypeHandle LerpNodePathInterval::type_handle_;
TypeHandle LerpPosInterval::type_handle_;
TypeHandle LerpHprInterval::type_handle_;
TypeHandle LerpQuatInterval::type_handle_;
TypeHandle LerpScaleInterval::type_handle_;
TypeHandle LerpShearInterval::type_handle_;
TypeHandle LerpPosHprInterval::type_handle_;

}
