#include "render_pipeline/rpcore/util/primitives.hpp"

#include <geomVertexWriter.h>
#include <geomTriangles.h>
#include <geomNode.h>
#include <materialAttrib.h>

#include "render_pipeline/rpcore/util/rpmaterial.hpp"

namespace rpcore {

static NodePath create_geom_node(const std::string& name, Geom* geom)
{
    // default material
    CPT(RenderState) state = RenderState::make_empty();
    state = state->add_attrib(MaterialAttrib::make(RPMaterial().get_material()));

    PT(GeomNode) geom_node = new GeomNode(name);
    geom_node->add_geom(geom, state);

    return NodePath(geom_node);
}

NodePath create_cube(const std::string& name)
{
    // create vertices
    PT(GeomVertexData) vdata = new GeomVertexData(name, GeomVertexFormat::get_v3n3t2(), Geom::UsageHint::UH_static);
    vdata->unclean_set_num_rows(24);

    GeomVertexWriter vertex(vdata, InternalName::get_vertex());
    GeomVertexWriter normal(vdata, InternalName::get_normal());
    GeomVertexWriter texcoord(vdata, InternalName::get_texcoord());

    vertex.add_data3f(-0.5f, -0.5f, -0.5f);     normal.add_data3f(0.0f, 0.0f, -1.0f);   texcoord.add_data2f(0.25f, 1/3.0f);     // 0
    vertex.add_data3f(-0.5f, -0.5f, -0.5f);     normal.add_data3f(0.0f, -1.0f, 0.0f);   texcoord.add_data2f(0.25f, 1/3.0f);     // 1
    vertex.add_data3f(-0.5f, -0.5f, -0.5f);     normal.add_data3f(-1.0f, 0.0f, 0.0f);   texcoord.add_data2f(0.25f, 1/3.0f);     // 2
    vertex.add_data3f(+0.5f, -0.5f, -0.5f);     normal.add_data3f(0.0f, 0.0f, -1.0f);   texcoord.add_data2f(0.5f, 1/3.0f);      // 3
    vertex.add_data3f(+0.5f, -0.5f, -0.5f);     normal.add_data3f(0.0f, -1.0f, 0.0f);   texcoord.add_data2f(0.5f, 1/3.0f);      // 4
    vertex.add_data3f(+0.5f, -0.5f, -0.5f);     normal.add_data3f(1.0f, 0.0f, 0.0f);    texcoord.add_data2f(0.5f, 1/3.0f);      // 5
    vertex.add_data3f(+0.5f, +0.5f, -0.5f);     normal.add_data3f(0.0f, 0.0f, -1.0f);   texcoord.add_data2f(0.5f, 0.0f);        // 6
    vertex.add_data3f(+0.5f, +0.5f, -0.5f);     normal.add_data3f(0.0f, 1.0f, 0.0f);    texcoord.add_data2f(0.75f, 1/3.0f);     // 7
    vertex.add_data3f(+0.5f, +0.5f, -0.5f);     normal.add_data3f(1.0f, 0.0f, 0.0f);    texcoord.add_data2f(0.75f, 1/3.0f);     // 8
    vertex.add_data3f(-0.5f, +0.5f, -0.5f);     normal.add_data3f(0.0f, 0.0f, -1.0f);   texcoord.add_data2f(0.25f, 0.0f);       // 9
    vertex.add_data3f(-0.5f, +0.5f, -0.5f);     normal.add_data3f(0.0f, 1.0f, 0.0f);    texcoord.add_data2f(1.0f, 1/3.0f);      // 10
    vertex.add_data3f(-0.5f, +0.5f, -0.5f);     normal.add_data3f(-1.0f, 0.0f, 0.0f);   texcoord.add_data2f(0.0f, 1/3.0f);      // 11
    vertex.add_data3f(-0.5f, -0.5f, +0.5f);     normal.add_data3f(0.0f, 0.0f, 1.0f);    texcoord.add_data2f(0.25f, 2/3.0f);     // 12
    vertex.add_data3f(-0.5f, -0.5f, +0.5f);     normal.add_data3f(0.0f, -1.0f, 0.0f);   texcoord.add_data2f(0.25f, 2/3.0f);     // 13
    vertex.add_data3f(-0.5f, -0.5f, +0.5f);     normal.add_data3f(-1.0f, 0.0f, 0.0f);   texcoord.add_data2f(0.25f, 2/3.0f);     // 14
    vertex.add_data3f(+0.5f, -0.5f, +0.5f);     normal.add_data3f(0.0f, 0.0f, 1.0f);    texcoord.add_data2f(0.5f, 2/3.0f);      // 15
    vertex.add_data3f(+0.5f, -0.5f, +0.5f);     normal.add_data3f(0.0f, -1.0f, 0.0f);   texcoord.add_data2f(0.5f, 2/3.0f);      // 16
    vertex.add_data3f(+0.5f, -0.5f, +0.5f);     normal.add_data3f(1.0f, 0.0f, 0.0f);    texcoord.add_data2f(0.5f, 2/3.0f);      // 17
    vertex.add_data3f(+0.5f, +0.5f, +0.5f);     normal.add_data3f(0.0f, 0.0f, 1.0f);    texcoord.add_data2f(0.5f, 1.0f);        // 18
    vertex.add_data3f(+0.5f, +0.5f, +0.5f);     normal.add_data3f(0.0f, 1.0f, 0.0f);    texcoord.add_data2f(0.75f, 2/3.0f);     // 19
    vertex.add_data3f(+0.5f, +0.5f, +0.5f);     normal.add_data3f(1.0f, 0.0f, 0.0f);    texcoord.add_data2f(0.75f, 2/3.0f);     // 20
    vertex.add_data3f(-0.5f, +0.5f, +0.5f);     normal.add_data3f(0.0f, 0.0f, 1.0f);    texcoord.add_data2f(0.25f, 1.0f);       // 21
    vertex.add_data3f(-0.5f, +0.5f, +0.5f);     normal.add_data3f(0.0f, 1.0f, 0.0f);    texcoord.add_data2f(1.0f, 2/3.0f);      // 22
    vertex.add_data3f(-0.5f, +0.5f, +0.5f);     normal.add_data3f(-1.0f, 0.0f, 0.0f);   texcoord.add_data2f(0.0f, 2/3.0f);      // 23

    // create indices
    PT(GeomTriangles) prim = new GeomTriangles(Geom::UsageHint::UH_static);
    prim->reserve_num_vertices(36);
    prim->add_vertices(0, 6, 3);
    prim->add_vertices(0, 9, 6);
    prim->add_vertices(12, 15, 18);
    prim->add_vertices(12, 18, 21);
    prim->add_vertices(22, 19, 7);
    prim->add_vertices(22, 7, 10);
    prim->add_vertices(13, 4, 16);
    prim->add_vertices(13, 1, 4);
    prim->add_vertices(14, 23, 11);
    prim->add_vertices(14, 11, 2);
    prim->add_vertices(17, 5, 8);
    prim->add_vertices(17, 8, 20);
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
        const double theta = i * pi / double(latitude);
        for (unsigned int j = 0; j <= longitude; ++j)
        {
            const double phi = j * 2.0 * pi / double(longitude);
            const double sin_theta = std::sin(theta);
            const double x = std::cos(phi) * sin_theta;
            const double y = std::sin(phi) * sin_theta;
            const double z = std::cos(theta);

            vertex.add_data3f(x, y, z);
            normal.add_data3f(x, y, z);
            texcoord.add_data2f(j / double(longitude), 1.0 - i / double(latitude));
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
