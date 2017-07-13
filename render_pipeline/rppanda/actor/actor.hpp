/**
 * This is C++ porting codes of direct/src/actor/Actor.py
 */

#pragma once

#include <nodePath.h>

#include <unordered_map>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

class Loader;

namespace rppanda {

/**
 * Actor class: Contains methods for creating, manipulating
 * and playing animations on characters.
 */
class RENDER_PIPELINE_DECL Actor: public DirectObject, public NodePath
{
public:
    using ModelsType = boost::variant<
            Filename,
            std::unordered_map<std::string, boost::variant<
                Filename,
                std::unordered_map<std::string, Filename>>>>;

    struct Parameters
    {
        boost::optional<ModelsType> models;
        bool copy = true;

        bool flattenable = true;
        boost::optional<bool> merge_LOD_bundles;
        boost::optional<bool> allow_async_bind;
        boost::optional<bool> ok_missing;
    };

    static std::string part_prefix;

    static LoaderOptions model_loader_options;
    static LoaderOptions anim_loader_options;

    static ConfigVariableBool validate_subparts;
    static ConfigVariableBool merge_LOD_bundles;
    static ConfigVariableBool allow_async_bind;

public:
    Actor(const Parameters& params);

    bool has_LOD(void) const;

    void set_geom_node(NodePath node);

    void load_model(NodePath model_path, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot",
        bool copy=true, bool auto_bind_anims=true);
    void load_model(const Filename& model_path, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot",
        bool copy=true, const boost::optional<bool>& ok_missing={}, bool auto_bind_anims=true);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
