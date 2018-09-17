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

#include "render_pipeline/rpcore/util/transparent_node.hpp"

#include <fmt/format.h>

#include <nodePathCollection.h>
#include <geomNode.h>
#include <materialAttrib.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/util/rpmaterial.hpp"

namespace rpcore {

const Effect::SourceType TransparentNode::effect_source = {
    "/$$rp/effects/default.yaml",
    { {"render_forward", true}, {"render_gbuffer", false} }
};

TransparentNode::TransparentNode(NodePath np) : np_(np)
{
}

void TransparentNode::set_effect(RenderPipeline& pipeline, int sort)
{
    pipeline.set_effect(np_, effect_source, sort);
}

void TransparentNode::set_effect_to_geometry(RenderPipeline& pipeline, int sort)
{
    NodePathCollection gn_npc = np_.find_all_matches("**/+GeomNode");
    if (np_.node()->is_of_type(GeomNode::get_class_type()))
        gn_npc.add_path(np_);

    for (int k = 0, k_end = gn_npc.get_num_paths(); k < k_end; ++k)
    {
        NodePath geom_np = gn_npc.get_path(k);
        GeomNode* geom_node = DCAST(GeomNode, geom_np.node());
        const int geom_count = geom_node->get_num_geoms();
        bool has_transparency = false;

        for (int i = 0; i < geom_count; ++i)
        {
            const RenderState* state = geom_node->get_geom_state(i);
            if (!state->has_attrib(MaterialAttrib::get_class_type()))
                continue;

            Material* material = DCAST(MaterialAttrib, state->get_attrib(MaterialAttrib::get_class_type()))->get_material();

            if (!has_transparency && RPMaterial(material).get_shading_model() == RPMaterial::ShadingModel::TRANSPARENT_MODEL)
                has_transparency = true;
        }

        // SHADING_MODEL_TRANSPARENT
        if (has_transparency)
        {
            if (geom_count > 1)
            {
                pipeline.error(fmt::format("Transparent materials must be on their own geom!\n"
                    "If you are exporting from blender, split them into\n"
                    "seperate meshes, then re-export your scene. The\n"
                    "problematic mesh is: {}", geom_np.get_name()));
                continue;
            }
            set_effect(pipeline, sort);
        }
    }
}

}