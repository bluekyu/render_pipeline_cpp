/**
 * Copyright (c) 2008, Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Carnegie Mellon University nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * (This is the Modified BSD License.  See also
 * http://www.opensource.org/licenses/bsd-license.php )
 */

/**
 * This is C++ porting codes of direct/src/actor/Actor.py
 */

#pragma once

#include <nodePath.h>
#include <partBundle.h>

#include <unordered_map>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

class Loader;
class PartBundleHandle;

namespace rppanda {

class ActorInterval;

/**
 * Actor class: Contains methods for creating, manipulating
 * and playing animations on characters.
 */
class RENDER_PIPELINE_DECL Actor : public DirectObject, public TypedReferenceCount, public NodePath
{
public:
    struct RENDER_PIPELINE_DECL Default
    {
        static constexpr const char* part_name = "modelRoot";
        static constexpr const char* lod_name = "lodRoot";
    };

public:
    /**
     * Instances of this class are stored within the
     * PartBundleDict to track all of the individual PartBundles
     * associated with the Actor.  In general, each separately loaded
     * model file is a different PartBundle.  This can include the
     * multiple different LOD's, as well as the multiple different
     * pieces of a multipart Actor.
     */
    class RENDER_PIPELINE_DECL PartDef
    {
    public:
        PartDef(NodePath part_bundle_np, PartBundleHandle* part_bundle_handle, PandaNode* part_model);

        PartBundle* get_bundle() const;

        NodePath part_bundle_np;
        PartBundleHandle* part_bundle_handle;
        PandaNode* part_model;
    };

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

    // { lod name, { part name, PartDef } }
    using PartBundleDictType = std::unordered_map<std::string, std::unordered_map<std::string, PartDef>>;

    static std::string part_prefix_;

    static LoaderOptions model_loader_options_;
    static LoaderOptions anim_loader_options_;

    static ConfigVariableBool validate_subparts_;
    static ConfigVariableBool merge_LOD_bundles_;
    static ConfigVariableBool allow_async_bind_;

public:
    /**
     * Create Actor instance.
     *
     * Actor constructor can be used to create single or multipart
     * actors. If another Actor is supplied as an argument this
     * method acts like a copy constructor. Single part actors are
     * created by calling with a model and animation dictionary
     * (animName, animPath{}) as follows:
     *
     * ```
     * PT(Actor) a = new Actor(
     *     Actor::ModelsType{"panda-3k.egg"},
     *     Actor::AnimsType{
     *         {"walk", "panda-walk.egg"},
     *         {"run", "panda-run.egg"}});
     * ```
     *
     * This could be displayed and animated as such:
     *
     * ```
     * a->reparent_to(render);
     * a->loop("walk");
     * a->stop();
     * ```
     */
    Actor(const boost::variant<void*, ModelsType, LODModelsType, MultiPartLODModelsType>& models=nullptr,
        const boost::variant<void*, AnimsType, MultiPartAnimsType>& anims=nullptr,
        boost::optional<NodePath> other = boost::none,
        bool copy=true,
        bool flattenable=true,
        bool set_final=false,
        boost::optional<bool> merge_LOD_bundles =boost::none,
        boost::optional<bool> allow_async_bind = boost::none,
        boost::optional<bool> ok_missing = boost::none
    );
    Actor(const Actor&) = delete;
    Actor(Actor&&) = default;

    virtual ~Actor();

    Actor& operator=(const Actor&) = delete;
    Actor& operator=(Actor&&) = default;

    ALLOC_DELETED_CHAIN(Actor);

    /**
     * Handy utility function to list the joint hierarchy of the actor.
     */
    void list_joints(const std::string& part_name=Default::part_name, const std::string& lod_name=Default::lod_name) const;

    /**
     * Utility function to create a list of information about an actor.
     * Useful for iterating over details of an actor.
     */
    ActorInfoType get_actor_info() const;

    std::vector<std::string> get_anim_names() const;

    /** Pretty print actor's details. */
    void pprint() const;

    /** accessing */
    ///@{

    PartBundleDictType& get_part_bundle_dict();
    const PartBundleDictType& get_part_bundle_dict() const;

    std::vector<PartBundle*> get_part_bundles(const boost::optional<std::string>& part_name = boost::none) const;

    /**
     * Return list of Actor LOD names. If not an LOD actor,
     * returns 'lodRoot'
     */
    const std::vector<std::string>& get_LOD_names() const;

    /** Return the node that contains all actor geometry. */
    NodePath get_geom_node() const;

    /** Set the node that contains all actor geometry */
    void set_geom_node(NodePath node);

    NodePath get_LOD_node() const;

    bool has_LOD() const;

    /**
     * Updates all of the Actor's joints in the indicated LOD.
     * The LOD may be specified by name, or by number, where 0 is the
     * highest level of detail, 1 is the next highest, and so on.
     *
     * If force is True, this will update every joint, even if we
     * don't believe it's necessary.
     *
     * Returns True if any joint has changed as a result of this,
     * False otherwise.
     */
    bool update(int lod=0, const boost::optional<std::string>& part_name = boost::none,
        const boost::optional<std::string>& lod_name = boost::none, bool force=false);

    /**
     * Return actual frame rate of given anim name and given part.
     * If no anim specified, use the currently playing anim.
     * If no part specified, return anim durations of first part.
     * NOTE: returns info only for an arbitrary LOD
     */
    boost::optional<double> get_frame_rate(const std::vector<std::string>& anim_name={}, const std::vector<std::string>& part_name={});
    boost::optional<double> get_any_frame_rate(const std::vector<std::string>& part_name = {});

    /**
     * Return frame rate of given anim name and given part, unmodified
     * by any play rate in effect.
     */
    boost::optional<double> get_base_frame_rate(const std::vector<std::string>& anim_name={}, const std::vector<std::string>& part_name={});
    boost::optional<double> get_any_base_frame_rate(const std::vector<std::string>& part_name = {});

    /**
     * Return the play rate of given anim for a given part.
     * If no part is given, assume first part in dictionary.
     * If no anim is given, find the current anim for the part.
     * NOTE: Returns info only for an arbitrary LOD
     */
    boost::optional<double> get_play_rate(const std::vector<std::string>& anim_name={}, const std::vector<std::string>& part_name={});
    boost::optional<double> get_any_play_rate(const std::vector<std::string>& part_name = {});

    /**
     * Set the play rate of given anim for a given part.
     * If no part is given, set for all parts in dictionary.
     *
     * It used to be legal to let the animName default to the
     * currently-playing anim, but this was confusing and could lead
     * to the wrong anim's play rate getting set.  Better to insist
     * on this parameter.
     * NOTE: sets play rate on all LODs
     */
    void set_play_rate(double rate, const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name={});
    void set_all_play_rate(double rate, const std::vector<std::string>& part_name = {});

    /**
     * Return duration of given anim name and given part.
     * If no anim specified, use the currently playing anim.
     * If no part specified, return anim duration of first part.
     * NOTE: returns info for arbitrary LOD
     */
    boost::optional<double> get_duration(const std::vector<std::string>& anim_name={}, const std::vector<std::string>& part_name={},
        boost::optional<double> from_frame=boost::none, boost::optional<double> to_frame=boost::none);
    boost::optional<double> get_any_duration(const std::vector<std::string>& part_name = {},
        boost::optional<double> from_frame = boost::none, boost::optional<double> to_frame = boost::none);

    boost::optional<int> get_num_frames(const std::vector<std::string>& anim_name={}, const std::vector<std::string>& part_name={});
    boost::optional<int> get_any_num_frames(const std::vector<std::string>& part_name = {});

    boost::optional<double> get_frame_time(const std::vector<std::string>& anim_name, double frame, const std::vector<std::string>& part_name={});
    boost::optional<double> get_any_frame_time(double frame, const std::vector<std::string>& part_name = {});

    ///@}

    /**
     * Returns a list of the AnimControl that represent the given or all
     * animations for the given part and the given lod.
     *
     * If @p anim_name is empty, the currently-playing
     * animation (or all currently-playing animations) is returned.
     * If @p anim_name is a single string name (the size of vector is one),
     * that particular animation is returned.
     * If @p anim_name is a vector of string names, all of the
     * names animations are returned.
     *
     * If part_name is none, all parts are returned (or
     * possibly the one overall Actor part, according to the
     * subpartsComplete flag).
     *
     * If lod_name is none, all LOD's are returned.
     */
    std::vector<AnimControl*> get_anim_controls(const std::vector<std::string>& anim_name={}, const std::vector<std::string>& part_name={},
        const boost::optional<std::string>& lod_name=boost::none, bool allow_async_bind=true);

    std::vector<AnimControl*> get_all_anim_controls(const std::vector<std::string>& part_name = {},
        const boost::optional<std::string>& lod_name = boost::none, bool allow_async_bind = true);

    /**
     * Actor model loader. Takes a model name (ie file path), a part
     * name (defaults to "modelRoot") and an lod name(defaults to "lodRoot").
     */
    void load_model(NodePath model_path, const std::string& part_name=Default::part_name, const std::string& lod_name=Default::lod_name,
        bool copy=true, bool auto_bind_anims=true);
    void load_model(const Filename& model_path, const std::string& part_name=Default::part_name, const std::string& lod_name=Default::lod_name,
        bool copy=true, const boost::optional<bool>& ok_missing=boost::none, bool auto_bind_anims=true);

    /**
     * loadAnims(self, string:string{}, string='modelRoot',
     * string='lodRoot')
     * Actor anim loader. Takes an optional partName (defaults to
     * 'modelRoot' for non-multipart actors) and lodName (defaults
     * to 'lodRoot' for non-LOD actors) and dict of corresponding
     * anims in the form animName:animPath{}
     */
    void load_anims(const AnimsType& anims, const std::string& part_name=Default::part_name, const std::string& lod_name=Default::lod_name);

    const PartDef* get_part_def(const std::string& part_name, const std::string& lod_name = Default::lod_name) const;

    /**
     * Find the named part in the optional named lod and return it, or
     * return None if not present.
     */
    NodePath get_part(const std::string& part_name, const std::string& lod_name = Default::lod_name) const;

    PartBundle* get_part_bundle(const std::string& part_name, const std::string& lod_name=Default::lod_name) const;

    /**
     * Make the given part of the optionally given lod not render,
     * even though still in the tree.
     * NOTE: this will affect child geometry
     */
    void hide_part(const std::string& part_name, const std::string& lod_name = Default::lod_name);

    /**
     * Make the given part render while in the tree.
     * NOTE: this will affect child geometry
     */
    void show_part(const std::string& part_name, const std::string& lod_name = Default::lod_name);

    /**
     * Make the given part and all its children render while in the tree.
     * NOTE: this will affect child geometry
     */
    void show_all_parts(const std::string& part_name, const std::string& lod_name = Default::lod_name);

    /**
     * Starts the joint animating the indicated node. As the joint
     * animates, it will transform the node by the corresponding
     * amount.  This will replace whatever matrix is on the node each
     * frame. The default is to expose the net transform from the root,
     * but if local_transform is true, only the node's local transform
     * from its parent is exposed.
     */
    NodePath expose_joint(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name=Default::lod_name, bool local_transform=false);

    boost::optional<LMatrix4f> get_joint_transform(const std::string& part_name, const std::string& joint_name, const std::string& lod_name = Default::lod_name) const;

    CPT(TransformState) get_joint_transform_state(const std::string& part_name, const std::string& joint_name, const std::string& lod_name = Default::lod_name) const;

    /**
     * The converse of expose_joint: this associates the joint with
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
    NodePath control_joint(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name=Default::lod_name);

    /**
     * Create tree of control joints from joint name
     *
     * @param[in]   node    Root node for the control joints. If it is emtpy, a new node will automatically
     *                      be created and loaded with the joint's initial transform.
     *
     * @return      Root node of the control joints. If failed, empty NodePath is returned.
     */
    NodePath control_joint_tree(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name = Default::lod_name);

    /**
     * Create tree of all control joints.
     *
     * @param[in]   root    Parent node for all control joints. If it is emtpy, parent will be the character node.
     *
     * @return      Parent node of all control joints. If @root is not empty, this is same as that.
     *              If failed, empty NodePath is returned.
     */
    NodePath control_all_joints(NodePath root, const std::string& part_name = Default::part_name, const std::string& lod_name = Default::lod_name);

    /**
     * Similar to control_joint, but the transform assigned is
     * static, and may not be animated at runtime (without another
     * subsequent call to freeze_joint).  This is slightly more
     * optimal than control_joint() for cases in which the transform
     * is not intended to be animated during the lifetime of the
     * Actor.
     */
    void freeze_joint(const std::string& part_name, const std::string& joint_name, CPT(TransformState) transform = nullptr,
        const LVecBase3f& pos = LVecBase3f(0), const LVecBase3f& hpr = LVecBase3f(0), const LVecBase3f& scale = LVecBase3f(1));

    /**
     * Undoes a previous call to control_joint() or freeze_joint()
     * and restores the named joint to its normal animation.
     */
    void release_joint(const std::string& part_name, const std::string& joint_name);

    /** actoins */
    ///@{

    /**
     * Stop named or all animations on the given part of the actor.
     *
     * NOTE: stops all LODs
     */
    void stop(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name={});
    void stop_all(const std::vector<std::string>& part_name = {});

    /**
     * Play the given or all animations on the given part of the actor.
     * If no part is specified, try to play on all parts. NOTE:
     * plays over ALL LODs
     */
    void play(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name={},
        boost::optional<double> from_frame=boost::none, boost::optional<double> to_frame=boost::none);

    void play_all(const std::vector<std::string>& part_name = {},
        boost::optional<double> from_frame = boost::none, boost::optional<double> to_frame = boost::none);

    /**
     * Loop the given or all animations on the given part of the actor,
     * restarting at zero frame if requested. If no part name
     * is given then try to loop on all parts. NOTE: loops on
     * all LOD's
     */
    void loop(const std::vector<std::string>& anim_name, bool restart=true, const std::vector<std::string>& part_name={},
        boost::optional<double> from_frame=boost::none, boost::optional<double> to_frame=boost::none);

    void loop_all(bool restart = true, const std::vector<std::string>& part_name = {},
        boost::optional<double> from_frame = boost::none, boost::optional<double> to_frame = boost::none);

    /**
     * Loops the given or all animations from the frame "from" to and including the frame "to",
     * and then back in the opposite direction, indefinitely. If no part name
     * is given then try to loop on all parts. NOTE: loops on
     * all LOD's
     */
    void pingpong(const std::vector<std::string>& anim_name, bool restart=true, const std::vector<std::string>& part_name={},
        boost::optional<double> from_frame=boost::none, boost::optional<double> to_frame=boost::none);

    void pingpong_all(bool restart = true, const std::vector<std::string>& part_name = {},
        boost::optional<double> from_frame = boost::none, boost::optional<double> to_frame = boost::none);

    /**
     * Pose the actor in position found at given frame in the specified or all
     * animations for the specified part. If no part is specified attempt
     * to apply pose to all parts.
     */
    void pose(const std::vector<std::string>& anim_name, double frame, const std::vector<std::string>& part_name={},
        const boost::optional<std::string>& lod_name=boost::none);

    void pose_all(double frame, const std::vector<std::string>& part_name = {},
        const boost::optional<std::string>& lod_name = boost::none);

    /**
     * Changes the way the Actor handles blending of multiple
     * different animations, and/or interpolation between consecutive
     * frames.
     *
     * The animBlend and frameBlend parameters are boolean flags.
     * You may set either or both to True or False.  If you do not
     * specify them, they do not change from the previous value.
     *
     * When animBlend is True, multiple different animations may
     * simultaneously be playing on the Actor.  This means you may
     * call play(), loop(), or pose() on multiple animations and have
     * all of them contribute to the final pose each frame.
     *
     * In this mode (that is, when animBlend is True), starting a
     * particular animation with play(), loop(), or pose() does not
     * implicitly make the animation visible; you must also call
     * setControlEffect() for each animation you wish to use to
     * indicate how much each animation contributes to the final
     * pose.
     *
     * The frameBlend flag is unrelated to playing multiple
     * animations.  It controls whether the Actor smoothly
     * interpolates between consecutive frames of its animation (when
     * the flag is True) or holds each frame until the next one is
     * ready (when the flag is False).  The default value of
     * frameBlend is controlled by the interpolate-frames Config.prc
     * variable.
     *
     * In either case, you may also specify blendType, which controls
     * the precise algorithm used to blend two or more different
     * matrix values into a final result.  Different skeleton
     * hierarchies may benefit from different algorithms.  The
     * default blendType is controlled by the anim-blend-type
     * Config.prc variable.
     */
    void set_blend(boost::optional<bool> anim_blend=boost::none, boost::optional<bool> frame_blend=boost::none,
        boost::optional<PartBundle::BlendType> blend_type=boost::none, const boost::optional<std::string>& part_name=boost::none);

    /**
     * Sets the amount by which the named or all animations contributes to
     * the overall pose.  This controls blending of multiple
     * animations; it only makes sense to call this after a previous
     * call to setBlend(animBlend = True).
     */
    void set_control_effect(const std::vector<std::string>& anim_name, float effect,
        const std::vector<std::string>& part_name = {},
        boost::optional<std::string> lod_name = boost::none);

    void set_all_control_effect(float effect,
        const std::vector<std::string>& part_name = {},
        boost::optional<std::string> lod_name = boost::none);

    ///@}

    /** Show the bounds of all actor geoms. */
    void show_all_bounds();

    /** Hide the bounds of all actor geoms. */
    void hide_all_bounds();

    /**
     * Binds the named or all animations to the named part and/or lod.  If
     * allow_async_bind is false, this guarantees that the animation is
     * bound immediately--the animation is never bound in a
     * sub-thread; it will be loaded and bound in the main thread, so
     * it will be available by the time this method returns.
     *
     * The parameters are the same as that for get_anim_controls().  In
     * fact, this method is a thin wrapper around that other method.
     *
     * Use this method if you need to ensure that an animation is
     * available before you start to play it, and you don't mind
     * holding up the render for a frame or two until the animation
     * is available.
     */
    void bind_anims(const std::vector<std::string>& anim_name,
        const std::vector<std::string>& part_name = {},
        boost::optional<std::string> lod_name = boost::none,
        bool allow_async_bind = false);

    void bind_all_anims(
        const std::vector<std::string>& part_name,
        boost::optional<std::string> lod_name = boost::none,
        bool allow_async_bind = false);

    /** Loads and binds all animations that have been defined for the Actor. */
    void bind_all_anims(bool allow_async_bind = false);

    PT(ActorInterval) actor_interval(const std::vector<std::string>& anim_name, bool loop=false,
        bool constrained_loop=false, boost::optional<double> duration=boost::none,
        boost::optional<double> start_time=boost::none, boost::optional<double> end_time=boost::none,
        boost::optional<double> start_frame=boost::none, boost::optional<double> end_frame=boost::none,
        double play_rate=1.0, const boost::optional<std::string> name=boost::none, bool force_update=false,
        const std::vector<std::string>& part_name={}, const boost::optional<std::string>& lod_name=boost::none);

protected:
    Loader* loader_ = nullptr;

    bool this_merge_LOD_bundles_;
    bool this_allow_async_bind_;

private:
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

        AnimDef make_copy();

        std::string filename;
        PT(AnimControl) anim_control;
        AnimBundle* anim_bundle;
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

    using CommonBundleHandlesType = std::unordered_map<std::string, PartBundleHandle*>;
    using SubpartDictType = std::unordered_map<std::string, SubpartDef>;

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

    void prepare_bundle(NodePath bundle_np, PandaNode* part_model, const std::string& part_name=Default::part_name, const std::string& lod_name=Default::lod_name);

    /**
     * Cache the sorted LOD names so we don't have to grab them
     * and sort them every time somebody asks for the list.
     */
    void update_sorted_LOD_names();

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

    std::vector<AnimControl*> get_anim_controls(bool is_all, const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name,
        const boost::optional<std::string>& lod_name, bool allow_async_bind);

    CommonBundleHandlesType common_bundle_handles_;
    SubpartDictType subpart_dict_;
    LODDictType anim_control_dict_;
    PartBundleDictType part_bundle_dict_;

    std::vector<std::string> sorted_LOD_names_;

    bool got_name_;
    NodePath geom_node_;
    NodePath LOD_node_;

    bool subparts_complete_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline Actor::PartDef::PartDef(NodePath part_bundle_np, PartBundleHandle* part_bundle_handle, PandaNode* part_model) :
    part_bundle_np(part_bundle_np), part_bundle_handle(part_bundle_handle), part_model(part_model)
{
}

inline Actor::PartBundleDictType& Actor::get_part_bundle_dict()
{
    return part_bundle_dict_;
}

inline const Actor::PartBundleDictType& Actor::get_part_bundle_dict() const
{
    return part_bundle_dict_;
}

inline const std::vector<std::string>& Actor::get_LOD_names() const
{
    return sorted_LOD_names_;
}

inline NodePath Actor::get_geom_node() const
{
    return geom_node_;
}

inline void Actor::set_geom_node(NodePath node)
{
    geom_node_ = node;
}

inline NodePath Actor::get_LOD_node() const
{
    return LOD_node_;
}

inline bool Actor::has_LOD() const
{
    return !LOD_node_.is_empty();
}

inline std::vector<AnimControl*> Actor::get_anim_controls(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name,
    const boost::optional<std::string>& lod_name, bool allow_async_bind)
{
    return get_anim_controls(false, anim_name, part_name, lod_name, allow_async_bind);
}

inline std::vector<AnimControl*> Actor::get_all_anim_controls(const std::vector<std::string>& part_name,
    const boost::optional<std::string>& lod_name, bool allow_async_bind)
{
    return get_anim_controls(true, {}, part_name, lod_name, allow_async_bind);
}

inline void Actor::stop(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name)
{
    for (auto&& control: get_anim_controls(anim_name, part_name))
        control->stop();
}

inline void Actor::stop_all(const std::vector<std::string>& part_name)
{
    for (auto&& control : get_all_anim_controls(part_name))
        control->stop();
}

inline void Actor::play(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    if (from_frame)
    {
        if (to_frame)
        {
            for (auto control: get_anim_controls(anim_name, part_name))
                control->play(from_frame.value(), to_frame.value());
        }
        else
        {
            for (auto control: get_anim_controls(anim_name, part_name))
                control->play(from_frame.value(), control->get_num_frames()-1);
        }
    }
    else
    {
        for (auto control: get_anim_controls(anim_name, part_name))
            control->play();
    }
}

inline void Actor::play_all(const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    if (from_frame)
    {
        if (to_frame)
        {
            for (auto control : get_all_anim_controls(part_name))
                control->play(from_frame.value(), to_frame.value());
        }
        else
        {
            for (auto control : get_all_anim_controls(part_name))
                control->play(from_frame.value(), control->get_num_frames() - 1);
        }
    }
    else
    {
        for (auto control : get_all_anim_controls(part_name))
            control->play();
    }
}

inline void Actor::loop(const std::vector<std::string>& anim_name, bool restart, const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    if (from_frame)
    {
        if (to_frame)
        {
            for (auto control: get_anim_controls(anim_name, part_name))
                control->loop(restart, from_frame.value(), to_frame.value());
        }
        else
        {
            for (auto control: get_anim_controls(anim_name, part_name))
                control->loop(restart, from_frame.value(), control->get_num_frames()-1);
        }
    }
    else
    {
        for (auto control: get_anim_controls(anim_name, part_name))
            control->loop(restart);
    }
}

inline void Actor::loop_all(bool restart, const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    if (from_frame)
    {
        if (to_frame)
        {
            for (auto control : get_all_anim_controls(part_name))
                control->loop(restart, from_frame.value(), to_frame.value());
        }
        else
        {
            for (auto control : get_all_anim_controls(part_name))
                control->loop(restart, from_frame.value(), control->get_num_frames() - 1);
        }
    }
    else
    {
        for (auto control : get_all_anim_controls(part_name))
            control->loop(restart);
    }
}

inline void Actor::pingpong(const std::vector<std::string>& anim_name, bool restart, const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    if (!from_frame)
        from_frame = 0;

    if (to_frame)
    {
        for (auto control: get_anim_controls(anim_name, part_name))
            control->pingpong(restart, from_frame.value(), to_frame.value());
    }
    else
    {
        for (auto control: get_anim_controls(anim_name, part_name))
            control->pingpong(restart, from_frame.value(), control->get_num_frames()-1);
    }
}

inline void Actor::pingpong_all(bool restart, const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    if (!from_frame)
        from_frame = 0;

    if (to_frame)
    {
        for (auto control : get_all_anim_controls(part_name))
            control->pingpong(restart, from_frame.value(), to_frame.value());
    }
    else
    {
        for (auto control : get_all_anim_controls(part_name))
            control->pingpong(restart, from_frame.value(), control->get_num_frames() - 1);
    }
}

inline void Actor::pose(const std::vector<std::string>& anim_name, double frame, const std::vector<std::string>& part_name,
    const boost::optional<std::string>& lod_name)
{
    for (auto control: get_anim_controls(anim_name, part_name, lod_name))
        control->pose(frame);
}

inline void Actor::pose_all(double frame, const std::vector<std::string>& part_name,
    const boost::optional<std::string>& lod_name)
{
    for (auto control : get_all_anim_controls(part_name, lod_name))
        control->pose(frame);
}

inline void Actor::set_blend(boost::optional<bool> anim_blend, boost::optional<bool> frame_blend,
    boost::optional<PartBundle::BlendType> blend_type, const boost::optional<std::string>& part_name)
{
    for (auto bundle: get_part_bundles(part_name))
    {
        if (blend_type)
            bundle->set_blend_type(blend_type.value());
        if (anim_blend)
            bundle->set_anim_blend_flag(anim_blend.value());
        if (frame_blend)
            bundle->set_frame_blend_flag(frame_blend.value());
    }
}

inline void Actor::bind_anims(const std::vector<std::string>& anim_name,
    const std::vector<std::string>& part_name,
    boost::optional<std::string> lod_name, bool allow_async_bind)
{
    get_anim_controls(anim_name, part_name, lod_name, allow_async_bind);
}

inline void Actor::bind_all_anims(const std::vector<std::string>& part_name,
    boost::optional<std::string> lod_name, bool allow_async_bind)
{
    get_all_anim_controls(part_name, lod_name, allow_async_bind);
}

inline void Actor::bind_all_anims(bool allow_async_bind)
{
    get_all_anim_controls({}, boost::none, allow_async_bind);
}

inline TypeHandle Actor::get_class_type()
{
    return type_handle_;
}

inline void Actor::init_type()
{
    TypedReferenceCount::init_type();
    register_type(type_handle_, "rppanda::Actor", TypedReferenceCount::get_class_type());
}

inline TypeHandle Actor::get_type() const
{
    return get_class_type();
}

inline TypeHandle Actor::force_init_type()
{
    init_type();
    return get_class_type();
}

}
