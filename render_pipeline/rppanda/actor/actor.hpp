/**
 * This is C++ porting codes of direct/src/actor/Actor.py
 */

#pragma once

#include <nodePath.h>
#include <partBundleHandle.h>

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

    using AnimInfoType = std::tuple<std::string, std::string, AnimControl*>;                // anim name, filename, AnimControl*
    using PartInfoType = std::tuple<std::string, PartBundle*, std::vector<AnimInfoType>>;   // part name, PartBundle*, AnimInfoType
    using LODInfoType = std::tuple<std::string, std::vector<PartInfoType>>;                 // lod name, PartInfoType
    using ActorInfoType = std::vector<LODInfoType>;

    static std::string part_prefix_;

    static LoaderOptions model_loader_options_;
    static LoaderOptions anim_loader_options_;

    static ConfigVariableBool validate_subparts_;
    static ConfigVariableBool merge_LOD_bundles_;
    static ConfigVariableBool allow_async_bind_;

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
    Actor(const Actor&) = delete;
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    Actor(Actor&&) = default;
#endif

    ~Actor(void) = default;

    Actor& operator=(const Actor&) = delete;
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    Actor& operator=(Actor&&);
#endif

    /**
     * Handy utility function to list the joint hierarchy of the actor.
     */
    void list_joints(const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot") const;

    /**
     * Utility function to create a list of information about an actor.
     * Useful for iterating over details of an actor.
     */
    ActorInfoType get_actor_info(void) const;

    std::vector<std::string> get_anim_names(void) const;

    /** Pretty print actor's details. */
    void pprint(void) const;

    /** accessing */
    ///@{

    /**
     * Return list of Actor LOD names. If not an LOD actor,
     * returns 'lodRoot'
     */
    const std::vector<std::string>& get_LOD_names(void) const;

    /** Return the node that contains all actor geometry. */
    NodePath get_geom_node(void) const;

    /** Set the node that contains all actor geometry */
    void set_geom_node(NodePath node);

    NodePath get_LOD_node(void) const;

    bool has_LOD(void) const;

    ///@}

    /**
     * getAnimControls(self, string, string=None, string=None)
     *
     * Returns a list of the AnimControls that represent the given
     * animation for the given part and the given lod.
     *
     * If animName is None or omitted, the currently-playing
     * animation (or all currently-playing animations) is returned.
     * If animName is True, all animations are returned.  If animName
     * is a single string name, that particular animation is
     * returned.  If animName is a list of string names, all of the
     * names animations are returned.
     *
     * If partName is None or omitted, all parts are returned (or
     * possibly the one overall Actor part, according to the
     * subpartsComplete flag).
     *
     * If lodName is None or omitted, all LOD's are returned.
     */
    std::vector<AnimControl*> get_anim_controls(const std::vector<std::string>& anim_name={}, const std::vector<std::string>& part_name={},
        const boost::optional<std::string>& lod_name={}, bool allow_async_bind=true);

    std::vector<AnimControl*> get_anim_controls(bool anim_name, const std::vector<std::string>& part_name={},
        const boost::optional<std::string>& lod_name={}, bool allow_async_bind=true);

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

    /**
     * exposeJoint(self, NodePath, string, string, key="lodRoot")
     * Starts the joint animating the indicated node.  As the joint
     * animates, it will transform the node by the corresponding
     * amount.  This will replace whatever matrix is on the node each
     * frame.  The default is to expose the net transform from the root,
     * but if localTransform is true, only the node's local transform
     * from its parent is exposed.
     */
    NodePath expose_joint(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name="lodRoot", bool local_transform=false);

    /**
     * The converse of exposeJoint: this associates the joint with
     * the indicated node, so that the joint transform will be copied
     * from the node to the joint each frame.  This can be used for
     * programmer animation of a particular joint at runtime.
     * 
     * The parameter node should be the NodePath for the node whose
     * transform will animate the joint.  If node is None, a new node
     * will automatically be created and loaded with the joint's
     * initial transform.  In either case, the node used will be
     * returned.
     * 
     * It used to be necessary to call this before any animations
     * have been loaded and bound, but that is no longer so.
     */
    NodePath control_joint(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name="lodRoot");

    /** Show the bounds of all actor geoms. */
    void show_all_bounds(void);

    /** Hide the bounds of all actor geoms. */
    void hide_all_bounds(void);

protected:
    Loader* loader_;

    bool this_merge_LOD_bundles_;
    bool this_allow_async_bind_;

private:
    /**
     * Instances of this class are stored within the
     * PartBundleDict to track all of the individual PartBundles
     * associated with the Actor.  In general, each separately loaded
     * model file is a different PartBundle.  This can include the
     * multiple different LOD's, as well as the multiple different
     * pieces of a multipart Actor.
     */
    class PartDef
    {
    public:
        PartBundle* get_bundle(void) const;

        NodePath part_bundle_np;
        PT(PartBundleHandle) part_bundle_handle;
        PT(PandaNode) part_model;
    };

    /**
     * Instances of this class are stored within the
     * AnimControlDict to track all of the animations associated with
     * the Actor.  This includes animations that have already been
     * bound (these have a valid AnimControl) as well as those that
     * have not yet been bound (for these, self->animControl is None).
     *
     * There is a different AnimDef for each different part or
     * sub-part, times each different animation in the AnimDict.
     */
    class AnimDef
    {
    public:
        AnimDef(const std::string& filename="", AnimBundle* anim_bundle=nullptr);

        AnimDef make_copy(void);

        std::string filename;
        PT(AnimControl) anim_control;
        PT(AnimBundle) anim_bundle;
    };

    /**
     * Instances of this class are stored within the SubpartDict
     * to track the existance of arbitrary sub-parts.  These are
     * designed to appear to the user to be identical to true "part"
     * of a multi-part Actor, but in fact each subpart represents a
     * subset of the joints of an existing part (which is accessible
     * via a different name).
     */
    class SubpartDef
    {
    public:
        SubpartDef(const std::string& true_part_name, const PartSubset& subset=PartSubset{});

        std::string true_part_name;
        PartSubset subset;
    };

    using AnimDictType = std::unordered_map<std::string, AnimDef>;
    using PartDictType = std::unordered_map<std::string, AnimDictType>;
    using LODDictType = std::unordered_map<std::string, PartDictType>;

private:
    void build_LOD_dict_items(std::vector<std::string>& part_name_list, LODDictType::iterator& lod_begin, LODDictType::iterator& lod_end, const boost::optional<std::string>& lod_name);
    void build_anim_dict_items(std::vector<PartDictType::iterator>& anim_dict_items, const std::vector<std::string>& part_name_list, PartDictType& part_dict);
    bool build_controls_from_anim_name(std::vector<AnimControl*>& controls, const std::vector<std::string>& names, AnimDictType& anim_dict, PartDictType& part_dict, const std::vector<std::string>& part_name_list,
        const std::string& part_name, const std::string& lod_name, bool allow_async_bind);

    void do_list_joints(size_t indent_level, const PartGroup* part, bool is_included, const PartSubset& subset) const;

    void post_load_model(NodePath model, const std::string& part_name, const std::string& lod_name, bool auto_bind_anims);

    void prepare_bundle(NodePath bundle_np, PandaNode* part_model, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot");

    /**
     * Cache the sorted LOD names so we don't have to grab them
     * and sort them every time somebody asks for the list.
     */
    void update_sorted_LOD_names(void);

    /**
     * Binds the named animation to the named part/lod and returns
     * the associated animControl.  The animation is loaded and bound
     * in a sub-thread, if allowAsyncBind is True,
     * self.allowAsyncBind is True, threading is enabled, and the
     * animation has a preload table generated for it (e.g. via
     * "egg-optchar -preload").  Even though the animation may or may
     * not be yet bound at the time this function returns, a usable
     * animControl is returned, or None if the animation could not be
     * bound.
     */
    AnimControl* bind_anim_to_part(const std::string& anim_name, const std::string& part_name, const std::string& lod_name, bool allow_async_bind=true);

    std::unordered_map<std::string, PT(PartBundleHandle)> common_bundle_handles_;
    std::unordered_map<std::string, SubpartDef> subpart_dict_;
    LODDictType anim_control_dict_;
    std::unordered_map<std::string, std::unordered_map<std::string, PartDef>> part_bundle_dict_;

    std::vector<std::string> sorted_LOD_names_;

    bool got_name_;
    NodePath geom_node_;
    NodePath LOD_node_;

    bool subparts_complete_;

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline const std::vector<std::string>& Actor::get_LOD_names(void) const
{
    return sorted_LOD_names_;
}

inline NodePath Actor::get_geom_node(void) const
{
    return geom_node_;
}

inline NodePath Actor::get_LOD_node(void) const
{
    return LOD_node_;
}

inline bool Actor::has_LOD(void) const
{
    return !LOD_node_.is_empty();
}

inline TypeHandle Actor::get_class_type(void)
{
    return type_handle_;
}

inline void Actor::init_type(void)
{
    DirectObject::init_type();
    register_type(type_handle_, "rppanda::Actor", DirectObject::get_class_type());
}

inline TypeHandle Actor::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle Actor::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
