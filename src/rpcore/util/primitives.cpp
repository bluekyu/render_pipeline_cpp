/**
 * Render Pipeline C++
 *
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

#include "render_pipeline/rpcore/util/primitives.hpp"

#include <cardMaker.h>
#include <geomVertexWriter.h>
#include <geomLinestrips.h>
#include <geomTriangles.h>
#include <geomNode.h>
#include <materialAttrib.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/util/rpmaterial.hpp"
#include "render_pipeline/rpcore/util/rpgeomnode.hpp"
#include "render_pipeline/rpcore/logger_manager.hpp"

namespace rpcore {

static NodePath create_geom_node(const std::string& name, PT(Geom) geom)
{
    PT(GeomNode) geom_node = new GeomNode(name);
    geom_node->add_geom(geom);

    // default material
    NodePath np(geom_node);
    RPGeomNode gn(np);
    gn.set_material(0, RPMaterial());

    return np;
}

// ************************************************************************************************

NodePath create_line(const std::string& name, const std::vector<LVecBase3>& vertices,
    float thickness, GeomEnums::UsageHint vertex_buffer_hint)
{
    if (vertices.size() > (std::numeric_limits<int>::max)())
    {
        LoggerManager::get_instance().get_logger()->error(fmt::format("The size {} of points is more than the maximum size ({}).",
            vertices.size(), (std::numeric_limits<int>::max)()));
        return NodePath();
    }

    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3(), vertex_buffer_hint);
    PT(GeomLinestrips) prim = new GeomLinestrips(GeomEnums::UsageHint::UH_static);

    const int count = static_cast<int>(vertices.size());

    vdata->unclean_set_num_rows(count);
    vdata->modify_array(0)->modify_handle()->copy_data_from(
        reinterpret_cast<const unsigned char*>(vertices.data()),
        count * sizeof(std::remove_reference<decltype(vertices)>::type::value_type));

    prim->clear_vertices();
    prim->add_consecutive_vertices(0, count);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    CPT(RenderAttrib) thick = RenderModeAttrib::make(RenderModeAttrib::M_unchanged, thickness);
    CPT(RenderState) state = RenderState::make(thick);

    PT(GeomNode) geom_node = new GeomNode(name);
    geom_node->add_geom(geom, state);

    // default material
    NodePath np = NodePath(geom_node);
    RPGeomNode gn(geom_node);
    gn.set_material(0, RPMaterial());

    return np;
}

NodePath create_triangle_mesh(
    const std::string& name,
    const std::vector<LVecBase3>& vertices,
    const std::vector<LVecBase3>& normals,
    const std::vector<LVecBase2>& texcoords,
    GeomEnums::UsageHint vertex_buffer_hint, GeomEnums::UsageHint index_buffer_hint)
{
    if (!(vertices.size() == normals.size() && normals.size() == texcoords.size()))
    {
        LoggerManager::get_instance().get_logger()->error("The counts of vertices, normal, texcoords is NOT same.");
        return NodePath();
    }

    if (vertices.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
    {
        LoggerManager::get_instance().get_logger()->error(fmt::format(
            "The number {} of vertices is more than the maximum size ({}).",
            vertices.size(),
            (std::numeric_limits<int>::max)()));
    }

    // Add vertices
    int vertices_count = static_cast<int>(vertices.size());

    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3n3t2(), vertex_buffer_hint);
    vdata->unclean_set_num_rows(vertices_count);

    GeomVertexWriter geom_vertices(vdata, InternalName::get_vertex());
    GeomVertexWriter geom_normals(vdata, InternalName::get_normal());
    GeomVertexWriter geom_texcoord0(vdata, InternalName::get_texcoord());

    for (int k = 0, k_end = vertices_count; k < k_end; ++k)
    {
        const auto& v = vertices[k];
        const auto& n = normals[k];
        const auto& t = texcoords[k];

        geom_vertices.add_data3(v[0], v[1], v[2]);
        geom_normals.add_data3(n[0], n[1], n[2]);
        geom_texcoord0.add_data2(t[0], t[1]);
    }

    // Add indices
    PT(GeomTriangles) prim = new GeomTriangles(index_buffer_hint);
    prim->add_consecutive_vertices(0, vertices_count);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    return create_geom_node(name, geom);
}

NodePath create_triangle_mesh(
    const std::string& name,
    const std::vector<LVecBase3>& vertices,
    const std::vector<LVecBase3>& normals,
    const std::vector<LVecBase2>& texcoords,
    const std::vector<int>& indices,
    GeomEnums::UsageHint vertex_buffer_hint, GeomEnums::UsageHint index_buffer_hint)
{
    const size_t indices_per_triangle = 3;

    if (indices.size() % indices_per_triangle != 0)
    {
        LoggerManager::get_instance().get_logger()->error("The count of indices is NOT a multiple of 3.");
        return NodePath();
    }

    if (!(vertices.size() == normals.size() && normals.size() == texcoords.size()))
    {
        LoggerManager::get_instance().get_logger()->error("The counts of vertices, normal, texcoords is NOT same.");
        return NodePath();
    }

    if (vertices.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
    {
        LoggerManager::get_instance().get_logger()->error(fmt::format(
            "The number {} of vertices is more than the maximum size ({}).",
            vertices.size(),
            (std::numeric_limits<int>::max)()));
    }

    // Add vertices
    int vertices_count = static_cast<int>(vertices.size());

    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3n3t2(), vertex_buffer_hint);
    vdata->unclean_set_num_rows(vertices_count);

    GeomVertexWriter geom_vertices(vdata, InternalName::get_vertex());
    GeomVertexWriter geom_normals(vdata, InternalName::get_normal());
    GeomVertexWriter geom_texcoord0(vdata, InternalName::get_texcoord());

    for (int k = 0, k_end = vertices_count; k < k_end; ++k)
    {
        const auto& v = vertices[k];
        const auto& n = normals[k];
        const auto& t = texcoords[k];

        geom_vertices.add_data3(v[0], v[1], v[2]);
        geom_normals.add_data3(n[0], n[1], n[2]);
        geom_texcoord0.add_data2(t[0], t[1]);
    }

    // Add indices
    PT(GeomTriangles) prim = new GeomTriangles(index_buffer_hint);
    prim->reserve_num_vertices(indices.size());
    for (size_t k = 0, k_end = indices.size(); k < k_end; k += 3)
        prim->add_vertices(indices[k], indices[k+1], indices[k+2]);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    return create_geom_node(name, geom);
}

NodePath create_plane(const std::string& name)
{
    // create vertices
    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3n3t2(), Geom::UsageHint::UH_static);
    vdata->unclean_set_num_rows(4);

    GeomVertexWriter vertex(vdata, InternalName::get_vertex());
    GeomVertexWriter normal(vdata, InternalName::get_normal());
    GeomVertexWriter texcoord(vdata, InternalName::get_texcoord());

    vertex.add_data3f(-0.5f, -0.5f, 0.0f);     normal.add_data3f(0.0f, 0.0f, 1.0f);     texcoord.add_data2f(0, 0);  // 0
    vertex.add_data3f(+0.5f, -0.5f, 0.0f);     normal.add_data3f(0.0f, 0.0f, 1.0f);     texcoord.add_data2f(1, 0);  // 1
    vertex.add_data3f(+0.5f, +0.5f, 0.0f);     normal.add_data3f(0.0f, 0.0f, 1.0f);     texcoord.add_data2f(1, 1);  // 2
    vertex.add_data3f(-0.5f, +0.5f, 0.0f);     normal.add_data3f(0.0f, 0.0f, 1.0f);     texcoord.add_data2f(0, 1);  // 3

    // create indices
    PT(GeomTriangles) prim = new GeomTriangles(Geom::UsageHint::UH_static);
    prim->reserve_num_vertices(6);
    prim->add_vertices(0, 1, 2);
    prim->add_vertices(2, 3, 0);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    return create_geom_node(name, geom);
}

NodePath create_cube(const std::string& name)
{
    // create vertices
    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3n3t2(), Geom::UsageHint::UH_static);
    vdata->unclean_set_num_rows(24);

    GeomVertexWriter vertex(vdata, InternalName::get_vertex());
    GeomVertexWriter normal(vdata, InternalName::get_normal());
    GeomVertexWriter texcoord(vdata, InternalName::get_texcoord());

    // top
    vertex.add_data3f(+0.5f, +0.5f, +0.5f);     normal.add_data3f( 0.0f,  0.0f,  1.0f);     texcoord.add_data2f(0.5f, 1.0f);        // 0
    vertex.add_data3f(-0.5f, +0.5f, +0.5f);     normal.add_data3f( 0.0f,  0.0f,  1.0f);     texcoord.add_data2f(0.25f, 1.0f);       // 1
    vertex.add_data3f(+0.5f, -0.5f, +0.5f);     normal.add_data3f( 0.0f,  0.0f,  1.0f);     texcoord.add_data2f(0.5f, 2/3.0f);      // 2
    vertex.add_data3f(-0.5f, -0.5f, +0.5f);     normal.add_data3f( 0.0f,  0.0f,  1.0f);     texcoord.add_data2f(0.25f, 2/3.0f);     // 3

    // right
    vertex.add_data3f(+0.5f, +0.5f, -0.5f);     normal.add_data3f( 1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.75f, 1/3.0f);     // 4
    vertex.add_data3f(+0.5f, +0.5f, +0.5f);     normal.add_data3f( 1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.75f, 2/3.0f);     // 5
    vertex.add_data3f(+0.5f, -0.5f, -0.5f);     normal.add_data3f( 1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.5f, 1/3.0f);      // 6
    vertex.add_data3f(+0.5f, -0.5f, +0.5f);     normal.add_data3f( 1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.5f, 2/3.0f);      // 7

    // back
    vertex.add_data3f(-0.5f, +0.5f, -0.5f);     normal.add_data3f( 0.0f,  1.0f,  0.0f);     texcoord.add_data2f(1.0f, 1/3.0f);      // 8
    vertex.add_data3f(-0.5f, +0.5f, +0.5f);     normal.add_data3f( 0.0f,  1.0f,  0.0f);     texcoord.add_data2f(1.0f, 2/3.0f);      // 9
    vertex.add_data3f(+0.5f, +0.5f, +0.5f);     normal.add_data3f( 0.0f,  1.0f,  0.0f);     texcoord.add_data2f(0.75f, 2/3.0f);     // 10
    vertex.add_data3f(+0.5f, +0.5f, -0.5f);     normal.add_data3f( 0.0f,  1.0f,  0.0f);     texcoord.add_data2f(0.75f, 1/3.0f);     // 11

    // bottom
    vertex.add_data3f(-0.5f, -0.5f, -0.5f);     normal.add_data3f( 0.0f,  0.0f, -1.0f);     texcoord.add_data2f(0.25f, 1/3.0f);     // 12
    vertex.add_data3f(-0.5f, +0.5f, -0.5f);     normal.add_data3f( 0.0f,  0.0f, -1.0f);     texcoord.add_data2f(0.25f, 0.0f);       // 13
    vertex.add_data3f(+0.5f, -0.5f, -0.5f);     normal.add_data3f( 0.0f,  0.0f, -1.0f);     texcoord.add_data2f(0.5f, 1/3.0f);      // 14
    vertex.add_data3f(+0.5f, +0.5f, -0.5f);     normal.add_data3f( 0.0f,  0.0f, -1.0f);     texcoord.add_data2f(0.5f, 0.0f);        // 15

    // front
    vertex.add_data3f(-0.5f, -0.5f, -0.5f);     normal.add_data3f( 0.0f, -1.0f,  0.0f);     texcoord.add_data2f(0.25f, 1/3.0f);     // 16
    vertex.add_data3f(+0.5f, -0.5f, -0.5f);     normal.add_data3f( 0.0f, -1.0f,  0.0f);     texcoord.add_data2f(0.5f, 1/3.0f);      // 17
    vertex.add_data3f(-0.5f, -0.5f, +0.5f);     normal.add_data3f( 0.0f, -1.0f,  0.0f);     texcoord.add_data2f(0.25f, 2/3.0f);     // 18
    vertex.add_data3f(+0.5f, -0.5f, +0.5f);     normal.add_data3f( 0.0f, -1.0f,  0.0f);     texcoord.add_data2f(0.5f, 2/3.0f);      // 19

    // left
    vertex.add_data3f(-0.5f, -0.5f, -0.5f);     normal.add_data3f(-1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.25f, 1/3.0f);     // 20
    vertex.add_data3f(-0.5f, -0.5f, +0.5f);     normal.add_data3f(-1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.25f, 2/3.0f);     // 21
    vertex.add_data3f(-0.5f, +0.5f, -0.5f);     normal.add_data3f(-1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.0f, 1/3.0f);      // 22
    vertex.add_data3f(-0.5f, +0.5f, +0.5f);     normal.add_data3f(-1.0f,  0.0f,  0.0f);     texcoord.add_data2f(0.0f, 2/3.0f);      // 23

    // create indices
    PT(GeomTriangles) prim = new GeomTriangles(Geom::UsageHint::UH_static);
    prim->reserve_num_vertices(36);
    prim->add_vertices(0, 1, 2);
    prim->add_vertices(3, 2, 1);
    prim->add_vertices(4, 5, 7);
    prim->add_vertices(6, 4, 7);
    prim->add_vertices(10, 11, 9);
    prim->add_vertices(8, 9, 11);
    prim->add_vertices(12, 13, 14);
    prim->add_vertices(15, 14, 13);
    prim->add_vertices(16, 17, 18);
    prim->add_vertices(19, 18, 17);
    prim->add_vertices(20, 21, 22);
    prim->add_vertices(23, 22, 21);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    return create_geom_node(name, geom);
}

NodePath create_sphere(const std::string& name, unsigned int latitude, unsigned int longitude)
{
    latitude = (std::max)(1u, latitude);
    longitude = (std::max)(1u, longitude);

    // create vertices
    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3n3t2(), Geom::UsageHint::UH_static);
    vdata->unclean_set_num_rows((latitude + 1)*(longitude + 1));

    GeomVertexWriter vertex(vdata, InternalName::get_vertex());
    GeomVertexWriter normal(vdata, InternalName::get_normal());
    GeomVertexWriter texcoord(vdata, InternalName::get_texcoord());

    const double pi = std::acos(-1);
    for (unsigned int i = 0; i <= latitude; ++i)
    {
        const double theta = i * pi / static_cast<double>(latitude);
        for (unsigned int j = 0; j <= longitude; ++j)
        {
            const double phi = j * 2.0 * pi / static_cast<double>(longitude);
            const double sin_theta = std::sin(theta);
            const double x = std::cos(phi) * sin_theta;
            const double y = std::sin(phi) * sin_theta;
            const double z = std::cos(theta);

            vertex.add_data3f(x, y, z);
            normal.add_data3f(x, y, z);
            texcoord.add_data2f(j / static_cast<double>(longitude), 1.0 - i / static_cast<double>(latitude));
        }
    }

    // create indices
    PT(GeomTriangles) prim = new GeomTriangles(Geom::UsageHint::UH_static);
    prim->reserve_num_vertices(longitude * latitude * 6);

    for (unsigned int i = 0; i < latitude; ++i)
    {
        for (unsigned int j = 0; j < longitude; ++j)
        {
            const int a = i * (longitude + 1) + j;
            const int b = a + (longitude + 1);

            prim->add_vertices(a, b, a+1);
            prim->add_vertices(b, b+1, a+1);
        }
    }
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    return create_geom_node(name, geom);
}

}
