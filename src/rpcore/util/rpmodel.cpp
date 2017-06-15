#include <nodePath.h>
#include <modelRoot.h>

#include "render_pipeline/rpcore/util/rpmodel.hpp"

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rpcore/util/rpmaterial.hpp"

#include "rplibs/yaml.hpp"

namespace rpcore {

void RPModel::apply_meta_file(const std::string& file_path)
{
    Filename _file_path = file_path;
    if (file_path.empty())
    {
        if (!nodepath_.node()->is_of_type(ModelRoot::get_class_type()))
        {
            error(fmt::format("NodePath ({}) is NOT ModelRoot type.", nodepath_));
            return;
        }

        const Filename& model_path = DCAST(ModelRoot, nodepath_.node())->get_fullpath();
        _file_path = Filename(model_path.get_dirname()) / (model_path.get_basename_wo_extension() + ".meta.yaml");
    }

    YAML::Node node_root;
    if (!rplibs::load_yaml_file(_file_path, node_root))
    {
        return;
    }

    apply_meta_node(node_root);
}

void RPModel::apply_meta_node(const YAML::Node& yaml_node)
{
    if (!yaml_node)
        return;

    if (const auto& model_node = yaml_node["model"])
        apply_meta_model_node(nodepath_, model_node);
}

void RPModel::apply_meta_model_node(NodePath nodepath, const YAML::Node& yaml_node)
{
    if (nodepath.is_empty())
    {
        error("NodePath is emtpy!");
        return;
    }

    if (!yaml_node)
        return;

    trace(fmt::format("Parsing NodePath ({}) and YAML node ({}) ...", nodepath, YAML::Dump(yaml_node)));

    if (const auto& name_node = yaml_node["name"])
    {
        try
        {
            nodepath.set_name(name_node.as<std::string>());
        }
        catch (const YAML::Exception&)
        {
            error("Cannot convert 'name' node to string.");
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

        try
        {
            if (const auto& c = material_node["base_color"])
                mat.set_base_color(LColor(c[0].as<float>(), c[1].as<float>(), c[2].as<float>(), c[3].as<float>()));
        }
        catch (const YAML::Exception&)
        {
            error(fmt::format("Cannot parse YAML node: {}", YAML::Dump(yaml_node)));
        }

        nodepath.set_material(mat.get_material());
    }

    const auto& children_node = yaml_node["children"];
    if (children_node)
    {
        for (size_t k=0,k_end=children_node.size(); k < k_end; ++k)
            apply_meta_model_node(nodepath.get_child(k), children_node[k]);
    }
}

}
