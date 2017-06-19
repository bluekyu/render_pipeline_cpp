#include "render_pipeline/rpcore/util/rpmodel.hpp"

#include <modelRoot.h>

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rpcore/rpobject.hpp"
#include "render_pipeline/rpcore/util/rpmaterial.hpp"

#include "rplibs/yaml.hpp"

namespace rpcore {

struct RPModel::Impl
{
    struct MetaData
    {
        static void load_nodepath(NodePath nodepath, const YAML::Node& yaml_node);
        static void load_material(RPMaterial& material, const YAML::Node& yaml_node);
    };
};

void RPModel::Impl::MetaData::load_nodepath(NodePath nodepath, const YAML::Node& yaml_node)
{
    if (nodepath.is_empty())
    {
        RPObject::global_error("RPModel", "NodePath is emtpy!");
        return;
    }

    if (!yaml_node)
        return;

    RPObject::global_trace("RPModel", fmt::format("Parsing NodePath ({}) and YAML node ({}) ...", nodepath, YAML::Dump(yaml_node)));

    if (const auto& name_node = yaml_node["name"])
    {
        try
        {
            nodepath.set_name(name_node.as<std::string>());
        }
        catch (const YAML::Exception&)
        {
            RPObject::global_error("RPModel", "Cannot convert 'name' node to string.");
        }
    }

    if (const auto& material_node = yaml_node["material"])
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

        load_material(mat, material_node);

        nodepath.set_material(mat.get_material());
    }

    const auto& children_node = yaml_node["children"];
    if (children_node)
    {
        for (size_t k=0,k_end=children_node.size(); k < k_end; ++k)
            load_nodepath(nodepath.get_child(k), children_node[k]);
    }
}

void RPModel::Impl::MetaData::load_material(RPMaterial& material, const YAML::Node& yaml_node)
{
    try
    {
        if (const auto& c = yaml_node["base_color"])
            material.set_base_color(LColor(c[0].as<float>(), c[1].as<float>(), c[2].as<float>(), c[3].as<float>()));

        if (const auto& f = yaml_node["specular_ior"])
            material.set_specular_ior(f.as<float>());

        if (const auto& f = yaml_node["metallic"])
            material.set_metallic(f.as<float>());

        if (const auto& f = yaml_node["roughness"])
            material.set_roughness(f.as<float>());

        if (const auto& s = yaml_node["shading_model"])
        {
            RPMaterial::ShadingModel shading_model = RPMaterial::ShadingModel::DEFAULT_MODEL;
            const std::string shading_model_string(s.as<std::string>());

            if (shading_model_string == "default")
                shading_model = RPMaterial::ShadingModel::DEFAULT_MODEL;
            else if (shading_model_string == "emissive")
                shading_model = RPMaterial::ShadingModel::EMISSIVE_MODEL;
            else if (shading_model_string == "clearcoat")
                shading_model = RPMaterial::ShadingModel::CLEARCOAT_MODEL;
            else if (shading_model_string == "transparent")
                shading_model = RPMaterial::ShadingModel::TRANSPARENT_MODEL;
            else if (shading_model_string == "skin")
                shading_model = RPMaterial::ShadingModel::SKIN_MODEL;
            else if (shading_model_string == "foliage")
                shading_model = RPMaterial::ShadingModel::FOLIAGE_MODEL;
            else
                RPObject::global_error("RPModel", fmt::format("Invalid shading model: {}", shading_model_string));

            material.set_shading_model(shading_model);
        }

        if (const auto& f = yaml_node["normal_factor"])
            material.set_normal_factor(f.as<float>());

        if (const auto& f = yaml_node["alpha"])
            material.set_alpha(f.as<float>());
    }
    catch (const YAML::Exception&)
    {
        RPObject::global_error("RPModel", fmt::format("Cannot parse YAML node: {}", YAML::Dump(yaml_node)));
    }
}

// ************************************************************************************************
void RPModel::load_meta_file(const std::string& file_path)
{
    Filename _file_path = file_path;
    if (file_path.empty())
    {
        if (!nodepath_.node()->is_of_type(ModelRoot::get_class_type()))
        {
            RPObject::global_error("RPModel", fmt::format("NodePath ({}) is NOT ModelRoot type.", nodepath_));
            return;
        }

        const Filename& model_path = DCAST(ModelRoot, nodepath_.node())->get_fullpath();
        _file_path = Filename(model_path.get_dirname()) / (model_path.get_basename_wo_extension() + ".meta.yaml");
    }

    YAML::Node node_root;
    if (!rplibs::load_yaml_file(_file_path, node_root))
    {
        RPObject::global_warn("RPModel", fmt::format("Failed to load model meta file: {}", file_path));
        return;
    }

    load_meta_data(node_root);
}

void RPModel::load_meta_data(const YAML::Node& yaml_node)
{
    if (!yaml_node)
        return;

    if (const auto& model_node = yaml_node["nodepath"])
        Impl::MetaData::load_nodepath(nodepath_, model_node);
}

}
