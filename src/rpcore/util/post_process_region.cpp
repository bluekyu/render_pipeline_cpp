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

#include "render_pipeline/rpcore/util/post_process_region.hpp"

#include <geomVertexWriter.h>
#include <geomTriangles.h>
#include <geomPoints.h>
#include <omniBoundingVolume.h>
#include <orthographicLens.h>
#include <graphicsOutput.h>

namespace rpcore {

std::unique_ptr<PostProcessRegion> PostProcessRegion::make(GraphicsOutput* internal_buffer, bool use_point)
{
    return std::make_unique<PostProcessRegion>(internal_buffer, use_point);
}

std::unique_ptr<PostProcessRegion> PostProcessRegion::make(GraphicsOutput* internal_buffer, const LVecBase4f& dimensions, bool use_point)
{
    return std::make_unique<PostProcessRegion>(internal_buffer, dimensions, use_point);
}

PostProcessRegion::PostProcessRegion(GraphicsOutput* internal_buffer, bool use_point)
{
    region_ = internal_buffer->make_display_region();
    node = NodePath("RTRoot");

    if (use_point)
        make_single_point();
    else
        make_fullscreen_tri();
    make_fullscreen_cam();
    init_function_pointers();
}

PostProcessRegion::PostProcessRegion(GraphicsOutput* internal_buffer, const LVecBase4f& dimensions, bool use_point)
{
    region_ = internal_buffer->make_display_region(dimensions);
    node = NodePath("RTRoot");

    if (use_point)
        make_single_point();
    else
        make_fullscreen_tri();
    make_fullscreen_cam();
    init_function_pointers();
}

void PostProcessRegion::init_function_pointers()
{
    using namespace std::placeholders;

    set_sort = std::bind(&DisplayRegion::set_sort, region_, _1);
    disable_clears = std::bind(&DisplayRegion::disable_clears, region_);
    set_active = std::bind(&DisplayRegion::set_active, region_, _1);
    set_clear_depth_active = std::bind(&DisplayRegion::set_clear_depth_active, region_, _1);
    set_clear_depth = std::bind(&DisplayRegion::set_clear_depth, region_, _1);
    set_camera = std::bind(&DisplayRegion::set_camera, region_, _1);
    set_clear_color_active = std::bind(&DisplayRegion::set_clear_color_active, region_, _1);
    set_clear_color = std::bind(&DisplayRegion::set_clear_color, region_, _1);
    set_draw_callback = std::bind(&DisplayRegion::set_draw_callback, region_, _1);

    set_instance_count = std::bind(&NodePath::set_instance_count, geom_np_, _1);
    set_shader = std::bind(&NodePath::set_shader, geom_np_, _1, _2);
    set_attrib = std::bind(&NodePath::set_attrib, geom_np_, _1, _2);
}

void PostProcessRegion::make_fullscreen_tri()
{
    const GeomVertexFormat* vformat =  GeomVertexFormat::get_v3();
    PT(GeomVertexData) vdata = new GeomVertexData("vertices", vformat, Geom::UH_static);
    vdata->set_num_rows(3);
    GeomVertexWriter vwriter(vdata, "vertex");
    vwriter.add_data3f(-1, 0, -1);
    vwriter.add_data3f(3, 0, -1);
    vwriter.add_data3f(-1, 0, 3);
    PT(GeomTriangles) gtris = new GeomTriangles(Geom::UH_static);
    gtris->add_next_vertices(3);
    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(gtris);
    make_geom_node(geom);
}

void PostProcessRegion::make_single_point()
{
    const GeomVertexFormat* vformat = GeomVertexFormat::get_v3();
    PT(GeomVertexData) vdata = new GeomVertexData("vertices", vformat, Geom::UH_static);
    vdata->set_num_rows(1);
    GeomVertexWriter vwriter(vdata, "vertex");
    vwriter.add_data3f(0, 0, 0);
    PT(GeomPoints) gpoint = new GeomPoints(Geom::UH_static);
    gpoint->add_next_vertices(1);
    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(gpoint);
    make_geom_node(geom);
}

void PostProcessRegion::make_geom_node(Geom* geom)
{
    PT(GeomNode) geom_node = new GeomNode("gn");
    geom_node->add_geom(geom);
    geom_node->set_final(true);
    PT(OmniBoundingVolume) obv = new OmniBoundingVolume();
    geom_node->set_bounds(obv);
    geom_np_ = NodePath(geom_node);
    geom_np_.set_depth_test(false);
    geom_np_.set_depth_write(false);
    geom_np_.set_attrib(TransparencyAttrib::make(TransparencyAttrib::M_none), 10000);
    geom_np_.set_color(LColor(1));
    geom_np_.set_bin("unsorted", 10);
    geom_np_.reparent_to(node);
}

void PostProcessRegion::make_fullscreen_cam()
{
    PT(Camera) buffer_cam = new Camera("BufferCamera");
    PT(OrthographicLens) lens = new OrthographicLens;
    lens->set_film_size(2, 2);
    lens->set_film_offset(0, 0);
    lens->set_near_far(-100, 100);
    buffer_cam->set_lens(lens);
    PT(OmniBoundingVolume) obv = new OmniBoundingVolume();
    buffer_cam->set_cull_bounds(obv);
    camera = node.attach_new_node(buffer_cam);
    region_->set_camera(camera);
}

}
