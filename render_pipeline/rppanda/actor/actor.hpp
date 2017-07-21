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
    using ModelsType = Filename;    // single part actor
    using LODModelsType = std::unordered_map<std::string, Filename>;                // single part actor w/ LOD
    using MultiPartModelsType = LODModelsType;                                      // multi-part actor
    using MultiPartLODModelsType = std::unordered_map<std::string, LODModelsType>;  // multi part actor w/ LOD

    using AnimsType = std::unordered_map<std::string, boost::variant<Filename, NodePath>>;  // single part actor || single part actor w/ LOD
    using MultiPartAnimsType = std::unordered_map<std::string, AnimsType>;                  // multi part actor || multi part actor w/ LOD

    static std::string part_prefix;

    static LoaderOptions model_loader_options;
    static LoaderOptions anim_loader_options;

    static ConfigVariableBool validate_subparts;
    static ConfigVariableBool merge_LOD_bundles;
    static ConfigVariableBool allow_async_bind;

public:
    Actor(const boost::variant<void*, ModelsType, LODModelsType, MultiPartLODModelsType>& models=nullptr,
        const boost::variant<void*, AnimsType, MultiPartAnimsType>& anims=nullptr,
        boost::optional<NodePath> other={},
        bool copy=true,
        bool flattenable=true,
        bool set_final=false,
        boost::optional<bool> merge_LOD_bundles={},
        boost::optional<bool> allow_async_bind={},
        boost::optional<bool> ok_missing={}
    );
    ~Actor(void);

    bool has_LOD(void) const;

    void set_geom_node(NodePath node);

    void load_model(NodePath model_path, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot",
        bool copy=true, bool auto_bind_anims=true);
    void load_model(const Filename& model_path, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot",
        bool copy=true, const boost::optional<bool>& ok_missing={}, bool auto_bind_anims=true);

    /**
     * loadAnims(self, string:string{}, string='modelRoot',
     * string='lodRoot')
     * Actor anim loader. Takes an optional partName (defaults to
     * 'modelRoot' for non-multipart actors) and lodName (defaults
     * to 'lodRoot' for non-LOD actors) and dict of corresponding
     * anims in the form animName:animPath{}
     */
    void load_anims(const AnimsType& anims, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot");

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
