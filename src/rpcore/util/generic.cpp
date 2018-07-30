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

#include "render_pipeline/rpcore/util/generic.hpp"

#include <load_dso.h>

#include "render_pipeline/rppanda/util/filesystem.hpp"
#include "render_pipeline/rplibs/py_to_cpp.hpp"
#include "render_pipeline/rpcore/rpobject.hpp"

namespace rpcore {

LVecBase3 rgb_from_string(const std::string& text, float min_brightness)
{
    const size_t ohash = std::hash<std::string>{}(text);

    LVecBase3f rgb(
        (ohash & 0xFF),
        (ohash >> 8) & 0xFF,
        (ohash >> 16) & 0xFF);
    const float neg_inf = 1.0f - min_brightness;

    return ((rgb / 255.0f) * neg_inf) + min_brightness;
}

void snap_shadow_map(const LMatrix4f& mvp, NodePath cam_node, int resolution)
{
    auto _mvp = mvp;
    const LVecBase4f& base_point = _mvp.xform(LPoint4f(0, 0, 0, 1)) * 0.5f + 0.5f;
    float texel_size = 1.0f / static_cast<float>(resolution);
    float offset_x = rplibs::py_fmod(base_point.get_x(), texel_size);
    float offset_y = rplibs::py_fmod(base_point.get_y(), texel_size);
    _mvp.invert_in_place();
    const LVecBase4f& new_base = _mvp.xform(LPoint4f(
        (base_point.get_x() - offset_x) * 2.0 - 1.0,
        (base_point.get_y() - offset_y) * 2.0 - 1.0,
        (base_point.get_z()) * 2.0 - 1.0, 1));
    cam_node.set_pos(cam_node.get_pos() - new_base.get_xyz());
}

void* load_rpassimp()
{
    return load_dso(
        rppanda::convert_path(rppanda::get_library_location().parent_path()),
        Filename::dso_filename("librpassimp.so"));
}

}
