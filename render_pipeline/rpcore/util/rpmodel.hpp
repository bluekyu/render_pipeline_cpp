#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace YAML {
class Node;
}

namespace rpcore {

/**
 * Wrapper class of Panda3D Material for physically based material.
 */
class RENDER_PIPELINE_DECL RPModel: public RPObject
{
public:
    RPModel(void);
    RPModel(NodePath nodepath);

    void apply_meta_file(const std::string& file_path="");
    void apply_meta_node(const YAML::Node& yaml_node);

private:
    void apply_meta_model_node(NodePath nodepath, const YAML::Node& yaml_node);

    NodePath nodepath_;
};

// ************************************************************************************************
inline RPModel::RPModel(void): RPObject("RPModel")
{
}

inline RPModel::RPModel(NodePath nodepath): RPObject("RPModel"), nodepath_(nodepath)
{
}

}
