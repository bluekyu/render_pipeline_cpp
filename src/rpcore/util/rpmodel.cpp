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

// NOMINMAX
#include <flatbuffers/idl.h>
#include <flatbuffers/util.h>

#include "render_pipeline/rpcore/util/rpmodel.hpp"

#include <virtualFileSystem.h>
#include <modelRoot.h>

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rpcore/mount_manager.hpp"
#include "render_pipeline/rpcore/loader.hpp"
#include "render_pipeline/rpcore/rpobject.hpp"
#include "render_pipeline/rpcore/util/rpmaterial.hpp"
#include "render_pipeline/rpcore/util/rpgeomnode.hpp"
#include "render_pipeline/rppanda/util/filesystem.hpp"

#include "rplibs/yaml.hpp"
#include "rpcore/schema/model_root_schema_generated.h"

namespace rpcore {

static const std::string CONTEXT_NAME("RPModel");

class RPModel::Impl
{
public:
    struct MetaData
    {
        static void load_panda_node(NodePath nodepath, const PandaNodeSchema* panda_node_data);
        static void load_geom_node(NodePath nodepath, const GeomNodeSchema* geom_node_data);
        static void load_geom(RPGeomNode& geom_node, size_t index, const GeomSchema* geom_data);
        static void load_material(RPMaterial& material, const RPMaterialSchema* rpmaterial_data);
    };
};

void RPModel::Impl::MetaData::load_panda_node(NodePath nodepath, const PandaNodeSchema* panda_node_data)
{
    if (nodepath.is_empty())
    {
        RPObject::global_error(CONTEXT_NAME, "NodePath is emtpy!");
        return;
    }

    if (!panda_node_data)
    {
        RPObject::global_error(CONTEXT_NAME, "PandaNodeSchema is nullptr!");
        return;
    }

    RPObject::global_trace(CONTEXT_NAME, fmt::format("Parsing NodePath ({}) and PandaNodeSchema ...", nodepath));

    if (const auto name_data = panda_node_data->name())
        nodepath.set_name(name_data->str());

    if (const auto material_data = panda_node_data->material())
    {
        RPMaterial mat(nullptr);
        if (nodepath.has_material())
        {
            mat.set_material(nodepath.get_material());
        }
        else
        {
            mat.set_material(new Material);
            mat.set_default();
        }

        load_material(mat, material_data);

        nodepath.set_material(mat.get_material());
    }

    switch (panda_node_data->node_type())
    {
    case NodeTypeSchema::GeomNode:
        load_geom_node(nodepath, panda_node_data->geom_node());
        break;
    default:
        break;
    }

    const auto& children_node = panda_node_data->children();
    if (children_node)
    {
        for (size_t k=0,k_end=children_node->size(); k < k_end; ++k)
            load_panda_node(nodepath.get_child(k), (*children_node)[k]);
    }
}

void RPModel::Impl::MetaData::load_geom_node(NodePath nodepath, const GeomNodeSchema* geom_node_data)
{
    if (nodepath.is_empty())
    {
        RPObject::global_error(CONTEXT_NAME, "NodePath is emtpy!");
        return;
    }

    if (!nodepath.node()->is_geom_node())
    {
        RPObject::global_error(CONTEXT_NAME, "NodePath is not GeomNode!");
        return;
    }

    if (!geom_node_data)
    {
        RPObject::global_error(CONTEXT_NAME, "GeomNodeSchema is nullptr!");
        return;
    }

    RPGeomNode geom_node(nodepath);
    if (const auto geoms_data = geom_node_data->geoms())
    {
        for (size_t k=0,k_end=geoms_data->size(); k < k_end; ++k)
            load_geom(geom_node, k, (*geoms_data)[k]);
    }
}

void RPModel::Impl::MetaData::load_geom(RPGeomNode& gemo_node, size_t index, const GeomSchema* geom_data)
{
    if (const auto rpmeterial_data = geom_data->material())
    {
        RPMaterial mat = gemo_node.get_material(index);
        if (!mat.get_material())
            mat.set_material(new Material);
        load_material(mat, rpmeterial_data);
        gemo_node.set_material(index, mat);
    }
}

void RPModel::Impl::MetaData::load_material(RPMaterial& material, const RPMaterialSchema* rpmaterial_data)
{
    if (!rpmaterial_data)
        return;

    if (const auto c = rpmaterial_data->base_color())
        material.set_base_color(LColor(c->x(), c->y(), c->z(), c->w()));

    if (const auto f = rpmaterial_data->specular_ior())
        material.set_specular_ior(f);

    if (const auto& f = rpmaterial_data->metallic())
        material.set_metallic(f);

    if (const auto& f = rpmaterial_data->roughness())
        material.set_roughness(f);

    switch (const auto shading_model = rpmaterial_data->shading_model())
    {
    case ShadingModelSchema::None:
        break;

    default:
        material.set_shading_model(RPMaterial::ShadingModel(shading_model));
    }

    if (const auto& f = rpmaterial_data->normal_factor())
        material.set_normal_factor(f);

    if (const auto& f = rpmaterial_data->arbitrary0())
        material.set_arbitrary0(f);
}

// ************************************************************************************************
void RPModel::load_meta_file(const Filename& file_path)
{
    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();

    Filename file_path_ = file_path;
    if (file_path_.empty())
    {
        if (!nodepath_.node()->is_of_type(ModelRoot::get_class_type()))
        {
            RPObject::global_error(CONTEXT_NAME, fmt::format("NodePath ({}) is NOT ModelRoot type.", nodepath_));
            return;
        }

        const Filename& model_path = DCAST(ModelRoot, nodepath_.node())->get_fullpath();
        file_path_ = Filename(model_path.get_dirname()) / (model_path.get_basename_wo_extension() + ".meta.json");
    }

    RPObject::global_debug(CONTEXT_NAME, fmt::format("Try to read JSON meta file ({}).", file_path_.c_str()));

    std::string file_contents;
    if (!vfs->read_file(file_path_, file_contents, false))
    {
        RPObject::global_error(CONTEXT_NAME, fmt::format("Failed to read JSON meta file ({}).", file_path_.c_str()));
        return;
    }

    load_meta_data(file_contents);
}

void RPModel::load_meta_data(const std::string& json_string)
{
    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();

    const Filename schema_path = Filename("/$$rp/data/schema/model_root_schema.fbs");

    RPObject::global_debug(CONTEXT_NAME, fmt::format("Try to read schema file ({}).", schema_path.c_str()));

    std::string schema_conents;
    if (!vfs->read_file(schema_path, schema_conents, false))
    {
        RPObject::global_error(CONTEXT_NAME, fmt::format("Failed to read schema file ({}).", schema_path.c_str()));
        return;
    }

    const auto& schema_dir = rppanda::convert_path(schema_path).parent_path();

    flatbuffers::Parser parser;
    const char* include_directories[] = { schema_dir.generic_string().c_str(), nullptr };
    if (!parser.Parse(schema_conents.c_str(), include_directories))
    {
        RPObject::global_error(CONTEXT_NAME, fmt::format("Cannot parse schema file ({}): {}",
            schema_path.c_str(), parser.error_));
        return;
    }

    if (!parser.Parse(json_string.c_str()))
    {
        RPObject::global_error(CONTEXT_NAME, fmt::format("Cannot parse JSON meta contents ({}).", json_string));
        RPObject::global_error(CONTEXT_NAME, fmt::format("Parsing error: {}.", parser.error_));
        return;
    }

    auto model_root_data = GetModelRootSchema(parser.builder_.GetBufferPointer());

    if (auto root_node_data = model_root_data->root_node())
        Impl::MetaData::load_panda_node(nodepath_, root_node_data);
}

}
