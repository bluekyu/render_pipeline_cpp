#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.hpp>

namespace YAML {
class Node;
}

namespace rpcore {

/**
 * Wrapper class of Panda3D model node.
 */
class RENDER_PIPELINE_DECL RPModel
{
public:
    RPModel(void) = default;
    RPModel(NodePath nodepath);

    void load_meta_file(const std::string& file_path="");
    void load_meta_data(const YAML::Node& yaml_node);

private:
    // function only PImpl
    struct Impl;

    NodePath nodepath_;
};

// ************************************************************************************************
inline RPModel::RPModel(NodePath nodepath): nodepath_(nodepath)
{
}

}
