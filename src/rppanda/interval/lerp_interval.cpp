#include "render_pipeline/rppanda/interval/lerp_interval.h"

namespace rppanda {

int LerpNodePathInterval::lerp_node_path_num = 1;

TypeHandle LerpNodePathInterval::_type_handle;
TypeHandle LerpPosInterval::_type_handle;
TypeHandle LerpHprInterval::_type_handle;
TypeHandle LerpQuatInterval::_type_handle;
TypeHandle LerpScaleInterval::_type_handle;
TypeHandle LerpShearInterval::_type_handle;
TypeHandle LerpPosHprInterval::_type_handle;

}	// namespace rppanda