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

#include "render_pipeline/rppanda/actor/actor.hpp"

#include <loader.h>
#include <modelNode.h>
#include <character.h>
#include <animControlCollection.h>
#include <auto_bind.h>
#include <animBundleNode.h>
#include <partBundleHandle.h>

#include <cctype>
#include <unordered_set>

#include <fmt/ostream.h>

#include <render_pipeline/rppanda/interval/actor_interval.hpp>

#include "rppanda/actor/config_rppanda_actor.hpp"

namespace rppanda {

static void build_control_joint_tree(PartGroup* joint, NodePath parent)
{
    if (!joint)
    {
        rppanda_actor_cat.warning() << "Joint is nullptr." << std::endl;
        return;
    }

    // Skip if the non-joint
    if (joint->is_of_type(MovingPartMatrix::get_class_type()))
    {
        const auto& joint_name = joint->get_name();

        NodePath node = parent.attach_new_node(new ModelNode(joint_name));

        node.set_mat(DCAST(MovingPartMatrix, joint)->get_default_value());

        if (!joint->apply_control(node.node()))
            rppanda_actor_cat.warning() << "Cannot control joint " << joint_name << std::endl;

        parent = node;
    }

    for (int k = 0, k_end = joint->get_num_children(); k < k_end; ++k)
        build_control_joint_tree(joint->get_child(k), parent);
}

// ************************************************************************************************

PartBundle* Actor::PartDef::get_bundle() const
{
    return part_bundle_handle->get_bundle();
}

// ************************************************************************************************

const std::string Actor::Default::part_name("modelRoot");
const std::string Actor::Default::lod_name("lodRoot");

TypeHandle Actor::type_handle_;

std::string Actor::part_prefix_("__Actor_");

LoaderOptions Actor::model_loader_options_(LoaderOptions::LF_search | LoaderOptions::LF_report_errors || LoaderOptions::LF_convert_skeleton);
LoaderOptions Actor::anim_loader_options_(LoaderOptions::LF_search | LoaderOptions::LF_report_errors || LoaderOptions::LF_convert_anim);

ConfigVariableBool Actor::validate_subparts_("validate-subparts", true);
ConfigVariableBool Actor::merge_LOD_bundles_("merge-lod-bundles", true);
ConfigVariableBool Actor::allow_async_bind_("allow-async-bind", true);

Actor::Actor(const boost::variant<void*, ModelsType, LODModelsType, MultiPartLODModelsType>& models,
    const boost::variant<void*, AnimsType, MultiPartAnimsType>& anims,
    boost::optional<NodePath> other,
    bool copy,
    bool flattenable,
    bool set_final,
    boost::optional<bool> merge_LOD_bundles,
    boost::optional<bool> allow_async_bind,
    boost::optional<bool> ok_missing)
{
    loader_ = Loader::get_global_ptr();

    // Set the merge_LOD_bundles flag.  If this is true, all
    // different LOD's will be merged into a single common bundle
    // (joint hierarchy).  All LOD's will thereafter share the same
    // skeleton, even though they may have been loaded from
    // different egg files.  If this is false, LOD's will be kept
    // completely isolated, and each LOD will have its own
    // skeleton.

    // When this flag is true, __animControlDict has only one key,
    // ['common']; when it is false, __animControlDict has one key
    // per each LOD name.

    if (merge_LOD_bundles)
        this_merge_LOD_bundles_ = merge_LOD_bundles.value();
    else
        this_merge_LOD_bundles_ = Actor::merge_LOD_bundles_.get_value();

    // Set the allowAsyncBind flag.  If this is true, it enables
    // asynchronous animation binding.  This requires that you have
    // run "egg-optchar -preload" on your animation and models to
    // generate the appropriate AnimPreloadTable.

    if (allow_async_bind)
        this_allow_async_bind_ = allow_async_bind.value();
    else
        this_allow_async_bind_ = Actor::allow_async_bind_.get_value();

    subparts_complete_ = false;

    if (!other)
    {
        // act like a normal constructor

        // create base hierarchy
        got_name_ = false;

        PT(PandaNode) root;
        if (flattenable)
        {
            // If we want a flattenable Actor, don't create all
            // those ModelNodes, and the GeomNode is the same as
            // the root.
            root = new PandaNode("actor");
            NodePath::operator=(NodePath(root));
            set_geom_node(*this);
        }
        else
        {
            // A standard Actor has a ModelNode at the root, and
            // another ModelNode to protect the GeomNode.
            PT(ModelNode) model = new ModelNode("actor");
            root = model;

            model->set_preserve_transform(ModelNode::PreserveTransform::PT_local);
            NodePath::operator=(NodePath(root));
            set_geom_node(attach_new_node(new ModelNode("actorGeom")));
        }

        // do we have a map of models?
        if (models.which() == 2)
        {
            // TODO
            rppanda_actor_cat.error() << "This is NOT implemented: " << __FILE__ << ":" << __LINE__ << std::endl;
        }
        // else it is a single part actor
        else if (models.which() == 1)
        {
            load_model(boost::get<ModelsType>(models), Default::part_name, Default::lod_name, copy, ok_missing);
        }

        // load anims
        // make sure the actor has animations
        if (anims.which() == 2)     // if so, does it have a dictionary of dictionaries?
        {
            const auto& multipart_anims = boost::get<MultiPartAnimsType>(anims);

            // are the models a dict of dicts too?
            if (models.which() == 3)
            {
                // then we have a multi-part w/ LOD
                const auto& lod_models = boost::get<LODModelsType>(models);
                std::vector<std::string> sorted_keys;
                sorted_keys.reserve(lod_models.size());
                for (const auto& key_val: lod_models)
                    sorted_keys.push_back(key_val.first);
                std::sort(sorted_keys.begin(), sorted_keys.end());
                for (const auto& lod_name: sorted_keys)
                {
                    // iterate over both dicts
                    for (const auto& partname_anims: multipart_anims)
                        load_anims(partname_anims.second, partname_anims.first, lod_name);
                }
            }
            else if (models.which() == 1)
            {
                // then it must be multi-part w/o LOD
                for (const auto& partname_anims: multipart_anims)
                    load_anims(partname_anims.second, partname_anims.first);
            }
        }
        // then we have single-part w/ LOD
        else if (models.which() == 2)
        {
            const auto& lod_models = boost::get<LODModelsType>(models);
            std::vector<std::string> sorted_keys;
            sorted_keys.reserve(lod_models.size());
            for (const auto& key_val: lod_models)
                sorted_keys.push_back(key_val.first);
            std::sort(sorted_keys.begin(), sorted_keys.end());
            for (const auto& lod_name: sorted_keys)
                load_anims(boost::get<AnimsType>(anims), Default::part_name, lod_name);
        }
        // else it is single-part w/o LOD
        else if (anims.which() == 1)
        {
            load_anims(boost::get<AnimsType>(anims));
        }
    }
    else
    {
        // TODO
        rppanda_actor_cat.error() << "This is NOT implemented: " << __FILE__ << ":" << __LINE__ << std::endl;
    }

    if (set_final)
    {
        // If setFinal is true, the Actor will set its top bounding
        // volume to be the "final" bounding volume: the bounding
        // volumes below the top volume will not be tested.  If a
        // cull test passes the top bounding volume, the whole
        // Actor is rendered.

        // We do this partly because an Actor is likely to be a
        // fairly small object relative to the scene, and is pretty
        // much going to be all onscreen or all offscreen anyway;
        // and partly because of the Character bug that doesn't
        // update the bounding volume for pieces that animate away
        // from their original position.  It's disturbing to see
        // someone's hands disappear; better to cull the whole
        // object or none of it.

        geom_node_.node()->set_final(true);
    }
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
Actor& Actor::operator=(Actor&&) = default;
#endif

Actor::~Actor() = default;

void Actor::list_joints(const std::string& part_name, const std::string& lod_name) const
{
    std::reference_wrapper<const std::string> true_name(part_name);
    PartSubset subpart_subset;
    const auto& subpart_dict_iter = subpart_dict_.find(part_name);
    if (subpart_dict_iter != subpart_dict_.end())
    {
        true_name = std::cref(subpart_dict_iter->second.true_part_name);
        subpart_subset = subpart_dict_iter->second.subset;
    }

    PartBundle* part_def = nullptr;
    try
    {
        if (this_merge_LOD_bundles_)
        {
            part_def = common_bundle_handles_.at(true_name)->get_bundle();
        }
        else
        {
            const auto& part_bundle_dict_iter = part_bundle_dict_.find(lod_name);
            if (part_bundle_dict_iter == part_bundle_dict_.end())
            {
                rppanda_actor_cat.error() << "No lod named: " << lod_name << std::endl;
            }
            else
            {
                part_def = part_bundle_dict_iter->second.at(true_name).get_bundle();
            }
        }
    }
    catch (const std::out_of_range&)
    {
        rppanda_actor_cat.error() << "No part named: " << part_name << std::endl;
        return;
    }

    do_list_joints(0, part_def, subpart_subset.is_include_empty(), subpart_subset);
}

Actor::ActorInfoType Actor::get_actor_info() const
{
    std::vector<LODInfoType> lod_info;
    lod_info.reserve(anim_control_dict_.size());
    for (const auto& lodname_partdict: anim_control_dict_)
    {
        std::string lod_name = lodname_partdict.first;
        if (this_merge_LOD_bundles_)
            lod_name = sorted_LOD_names_[0];

        std::vector<PartInfoType> part_info;
        part_info.reserve(lodname_partdict.second.size());
        for (const auto& partname_animdict: lodname_partdict.second)
        {
            const std::string& part_name = partname_animdict.first;

            std::string true_part_name;
            const auto& subpart_dict_found = subpart_dict_.find(part_name);
            if (subpart_dict_found != subpart_dict_.end())
                true_part_name = subpart_dict_found->second.true_part_name;
            else
                true_part_name = part_name;

            const auto& part_bundle_dict = part_bundle_dict_.at(lod_name);
            const auto& part_def = part_bundle_dict.at(true_part_name);
            PartBundle* part_bundle = part_def.get_bundle();
            const auto& anim_dict = partname_animdict.second;

            std::vector<AnimInfoType> anim_info;
            anim_info.reserve(anim_dict.size());
            for (const auto& animname_anim: anim_dict)
            {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
                anim_info.push_back({animname_anim.first,
                    animname_anim.second.filename,
                    animname_anim.second.anim_control});
#else
                anim_info.push_back(AnimInfoType{animname_anim.first,
                    animname_anim.second.filename,
                    animname_anim.second.anim_control});
#endif
            }
#if !defined(_MSC_VER) || _MSC_VER >= 1900
            part_info.push_back({part_name, part_bundle, std::move(anim_info)});
#else
            part_info.push_back(PartInfoType{part_name, part_bundle, std::move(anim_info)});
#endif
        }
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        lod_info.push_back({lod_name, std::move(part_info)});
#else
        lod_info.push_back(LODInfoType{lod_name, std::move(part_info)});
#endif
    }
    return lod_info;
}

std::vector<std::string> Actor::get_anim_names() const
{
    std::unordered_set<std::string> anim_names;
    const auto& actor_info = get_actor_info();
    for (const auto& lod_info: actor_info)
    {
        for (const auto& part_info: std::get<1>(lod_info))
        {
            for (const auto& anim_info: std::get<2>(part_info))
            {
                anim_names.insert(std::get<0>(anim_info));
            }
        }
    }

    return std::vector<std::string>(anim_names.begin(), anim_names.end());
}

void Actor::pprint() const
{
    const auto& actor_info = get_actor_info();
    for (const auto& lod_info: actor_info)
    {
        std::cout << "LOD: " << std::get<0>(lod_info) << std::endl;
        for (const auto& part_info: std::get<1>(lod_info))
        {
            std::cout << "  Part: " << std::get<0>(part_info) << std::endl;
            std::cout << "  Bundle: " << *std::get<1>(part_info) << std::endl;
            for (const auto& anim_info: std::get<2>(part_info))
            {
                std::cout << "    Anim: " << std::get<0>(anim_info) << std::endl;
                std::cout << "      File: " << std::get<1>(anim_info) << std::endl;
                AnimControl* anim_control = std::get<2>(anim_info);
                if (anim_control)
                    std::cout << "      NumFrames: " << anim_control->get_num_frames() << " PlayRate: " << anim_control->get_play_rate() << std::endl;
                else
                    std::cout << "      (not loaded)" << std::endl;
            }
        }
    }
}

inline std::vector<PartBundle*> Actor::get_part_bundles(const boost::optional<std::string>& part_name) const
{
    std::vector<PartBundle*> bundles;

    for (const auto& key_val: part_bundle_dict_)
    {
        const auto& part_bundle_dict = key_val.second;

        if (part_name)
        {
            std::string true_part_name;
            const auto& subpart_dict_iter = subpart_dict_.find(part_name.value());
            if (subpart_dict_iter != subpart_dict_.end())
                true_part_name = subpart_dict_iter->second.true_part_name;
            else
                true_part_name = part_name.value();
            auto found = part_bundle_dict.find(true_part_name);
            if (found != part_bundle_dict.end())
                bundles.push_back(found->second.get_bundle());
            else
                rppanda_actor_cat.warning() << "Couldn't find part: " << part_name.value() << std::endl;
        }
        else
        {
            for (const auto& part_def: part_bundle_dict)
                bundles.push_back(part_def.second.get_bundle());
        }
    }

    return bundles;
}

bool Actor::update(int lod, const boost::optional<std::string>& part_name,
    const boost::optional<std::string>& lod_name, bool force)
{
    std::vector<std::string> lod_names;
    if (lod_name)
        lod_names.push_back(lod_name.value());
    else
        lod_names = get_LOD_names();

    bool any_changed = false;
    if (lod < static_cast<int>(lod_names.size()))
    {
        const auto& curr_lod_name = lod_names[lod];
        std::vector<std::string> part_names;
        if (part_name)
        {
            part_names.push_back(part_name.value());
        }
        else
        {
            const auto& part_bundle_dict = part_bundle_dict_.at(curr_lod_name);
            part_names.reserve(part_bundle_dict.size());
            for (const auto& part_bundle: part_bundle_dict)
                part_names.push_back(part_bundle.first);
        }

        for (const auto& part_name: part_names)
        {
            auto part_bundle = get_part_bundle(part_name, lod_names[lod]);
            if (force)
            {
                if (part_bundle->force_update())
                    any_changed = true;
            }
            else
            {
                if (part_bundle->update())
                    any_changed = true;
            }
        }
    }
    else
    {
        rppanda_actor_cat.warning() << "update() - no lod: " << lod << std::endl;
    }

    return any_changed;
}

boost::optional<double> Actor::get_frame_rate(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name)
{
    const auto& controls = get_anim_controls(anim_name, part_name);
    if (controls.empty())
        return {};

    return controls[0]->get_frame_rate();
}

boost::optional<double> Actor::get_frame_rate(bool, const std::vector<std::string>& part_name)
{
    const auto& controls = get_anim_controls(true, part_name);
    if (controls.empty())
        return {};

    return controls[0]->get_frame_rate();
}

boost::optional<double> Actor::get_base_frame_rate(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name)
{
    const auto& controls = get_anim_controls(anim_name, part_name);
    if (controls.empty())
        return {};

    return controls[0]->get_anim()->get_base_frame_rate();
}

boost::optional<double> Actor::get_base_frame_rate(bool, const std::vector<std::string>& part_name)
{
    const auto& controls = get_anim_controls(true, part_name);
    if (controls.empty())
        return {};

    return controls[0]->get_anim()->get_base_frame_rate();
}

boost::optional<double> Actor::get_play_rate(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name)
{
    if (!anim_control_dict_.empty())
    {
        // use the first lod
        const auto& controls = get_anim_controls(anim_name, part_name);
        if (!controls.empty())
            return controls[0]->get_play_rate();
    }
    return {};
}


boost::optional<double> Actor::get_play_rate(bool, const std::vector<std::string>& part_name)
{
    if (!anim_control_dict_.empty())
    {
        // use the first lod
        const auto& controls = get_anim_controls(true, part_name);
        if (!controls.empty())
            return controls[0]->get_play_rate();
    }
    return {};
}

void Actor::set_play_rate(double rate, const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name)
{
    for (auto control: get_anim_controls(anim_name, part_name))
        control->set_play_rate(rate);
}

void Actor::set_play_rate(double rate, bool, const std::vector<std::string>& part_name)
{
    for (auto control : get_anim_controls(true, part_name))
        control->set_play_rate(rate);
}

boost::optional<double> Actor::get_duration(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    const auto& controls = get_anim_controls(anim_name, part_name);
    if (controls.empty())
        return {};

    AnimControl* anim_control = controls[0];
    if (!from_frame)
        from_frame = 0;
    if (!to_frame)
        to_frame = anim_control->get_num_frames() - 1;
    return ((to_frame.value() + 1) - from_frame.value()) / anim_control->get_frame_rate();
}

boost::optional<double> Actor::get_duration(bool, const std::vector<std::string>& part_name,
    boost::optional<double> from_frame, boost::optional<double> to_frame)
{
    const auto& controls = get_anim_controls(true, part_name);
    if (controls.empty())
        return {};

    AnimControl* anim_control = controls[0];
    if (!from_frame)
        from_frame = 0;
    if (!to_frame)
        to_frame = anim_control->get_num_frames() - 1;
    return ((to_frame.value() + 1) - from_frame.value()) / anim_control->get_frame_rate();
}

boost::optional<int> Actor::get_num_frames(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name)
{
    const auto& controls = get_anim_controls(anim_name, part_name);
    if (controls.empty())
        return {};
    return controls[0]->get_num_frames();
}

boost::optional<int> Actor::get_num_frames(bool, const std::vector<std::string>& part_name)
{
    const auto& controls = get_anim_controls(true, part_name);
    if (controls.empty())
        return {};
    return controls[0]->get_num_frames();
}

boost::optional<double> Actor::get_frame_time(const std::vector<std::string>& anim_name, double frame, const std::vector<std::string>& part_name)
{
    auto num_frames = get_num_frames(anim_name, part_name);
    auto anim_time = get_duration(anim_name, part_name);
    if (!num_frames || !anim_time)
        return {};
    return anim_time.value() * frame / num_frames.value();
}

boost::optional<double> Actor::get_frame_time(bool, double frame, const std::vector<std::string>& part_name)
{
    auto num_frames = get_num_frames(true, part_name);
    auto anim_time = get_duration(true, part_name);
    if (!num_frames || !anim_time)
        return {};
    return anim_time.value() * frame / num_frames.value();
}

std::vector<AnimControl*> Actor::get_anim_controls(const std::vector<std::string>& anim_name, const std::vector<std::string>& part_name,
    const boost::optional<std::string>& lod_name, bool allow_async_bind)
{
    std::vector<AnimControl*> controls;
    LODDictType::iterator iter;
    LODDictType::iterator iter_end;

    std::vector<std::string> part_name_list = part_name;
    build_LOD_dict_items(part_name_list, iter, iter_end, lod_name);

    for (; iter != iter_end; ++iter)
    {
        const std::string& lod_name = iter->first;
        auto& part_dict = iter->second;

        std::vector<PartDictType::iterator> anim_dict_items;

        build_anim_dict_items(anim_dict_items, part_name_list, part_dict);

        if (anim_name.empty())
        {
            // get all playing animations
            for (const auto& iter: anim_dict_items)
            {
                for (const auto& key_anim: iter->second)
                {
                    if (key_anim.second.anim_control && key_anim.second.anim_control->is_playing())
                        controls.push_back(key_anim.second.anim_control);
                }
            }
        }
        else
        {
            // get the named animation(s) only.
            for (auto&& iter: anim_dict_items)
            {
                auto& anim_dict = iter->second;

                if (!build_controls_from_anim_name(controls, anim_name, anim_dict, part_dict, part_name_list, iter->first, lod_name, allow_async_bind))
                    return {};
            }
        }
    }

    return controls;
}

std::vector<AnimControl*> Actor::get_anim_controls(bool, const std::vector<std::string>& part_name,
    const boost::optional<std::string>& lod_name, bool allow_async_bind)
{
    std::vector<AnimControl*> controls;
    LODDictType::iterator iter;
    LODDictType::iterator iter_end;

    std::vector<std::string> part_name_list = part_name;
    build_LOD_dict_items(part_name_list, iter, iter_end, lod_name);

    for (; iter != iter_end; ++iter)
    {
        const std::string& lod_name = iter->first;
        auto& part_dict = iter->second;

        std::vector<PartDictType::iterator> anim_dict_items;

        build_anim_dict_items(anim_dict_items, part_name_list, part_dict);

        // get the named animation(s) only.
        for (auto&& iter: anim_dict_items)
        {
            auto& anim_dict = iter->second;

            // anim_name: True to indicate all anims.
            std::vector<std::string> names;
            names.reserve(anim_dict.size());
            for (auto&& key_val: anim_dict)
                names.push_back(key_val.first);

            if (!build_controls_from_anim_name(controls, names, anim_dict, part_dict, part_name_list, iter->first, lod_name, allow_async_bind))
                return {};
        }
    }

    return controls;
}

void Actor::load_model(NodePath model_path, const std::string& part_name, const std::string& lod_name,
    bool copy, bool auto_bind_anims)
{
    if (subpart_dict_.find(part_name) != subpart_dict_.end())
    {
        rppanda_actor_cat.error() << "Part (" << part_name << ") is already loaded." << std::endl;
        return;
    }

    rppanda_actor_cat.debug() << fmt::format("in load_model: {}, part: {}, lod: {}, copy: {}", model_path, part_name, lod_name, copy) << std::endl;

    NodePath model;
    if (copy)
        model = model_path.copy_to(NodePath());
    else
        model = model_path;

    post_load_model(model, part_name, lod_name, auto_bind_anims);
}

void Actor::load_model(const Filename& model_path, const std::string& part_name, const std::string& lod_name,
    bool copy, const boost::optional<bool>& ok_missing, bool auto_bind_anims)
{
    if (subpart_dict_.find(part_name) != subpart_dict_.end())
    {
        rppanda_actor_cat.error() << "Part (" << part_name << ") is already loaded." << std::endl;
        return;
    }

    rppanda_actor_cat.debug() << fmt::format("in load_model: {}, part: {}, lod: {}, copy: {}", model_path, part_name, lod_name, copy) << std::endl;

#if !defined(_MSC_VER) || _MSC_VER >= 1900
    std::shared_ptr<LoaderOptions> loader_options = std::shared_ptr<LoaderOptions>(&Actor::model_loader_options_, [](auto){});
#else
    std::shared_ptr<LoaderOptions> loader_options = std::shared_ptr<LoaderOptions>(&Actor::model_loader_options_, [](LoaderOptions*){});
#endif

    if (!copy)
    {
        // If copy = 0, then we should always hit the disk.
        loader_options = std::make_shared<LoaderOptions>(Actor::model_loader_options_);
        loader_options->set_flags(loader_options->get_flags() & ~LoaderOptions::LF_no_ram_cache);
    }

    if (ok_missing)
    {
        if (ok_missing.value())
            loader_options->set_flags(loader_options->get_flags() & ~LoaderOptions::LF_report_errors);
        else
            loader_options->set_flags(loader_options->get_flags() | LoaderOptions::LF_report_errors);
    }

    // Pass loaderOptions to specify that we want to
    // get the skeleton model.  This only matters to model
    // files (like .mb) for which we can choose to extract
    // either the skeleton or animation, or neither.
    PT(PandaNode) model_node = loader_->load_sync(model_path, *loader_options);
    NodePath model;
    if (model_node)
        model = NodePath(model_node);

    if (model.is_empty())
    {
        rppanda_actor_cat.error() << "Could not load Actor model from " << model_path << std::endl;
        return;
    }

    post_load_model(model, part_name, lod_name, auto_bind_anims);
}

void Actor::load_anims(const AnimsType& anims, const std::string& part_name, const std::string& lod_name)
{
    bool reload = true;
    std::vector<std::string> lod_names;
    if (this_merge_LOD_bundles_)
    {
        lod_names.push_back("common");
    }
    else if (lod_name == "all")
    {
        reload = false;
        // TODO
        rppanda_actor_cat.error() << "This is NOT implemented: " << __FILE__ << ":" << __LINE__ << std::endl;
    }
    else
    {
        lod_names.push_back(lod_name);
    }

    if (rppanda_actor_cat.is_on(NotifySeverity::NS_debug))
    {
        std::stringstream ss;
        ss << "{";
        for (const auto& key_val: anims)
            ss << key_val.first << ":" << key_val.second << ", ";
        ss << "}";

        rppanda_actor_cat.debug() << fmt::format("in load_anims: {}, part: {}, lod: {}", ss.str(), part_name, lod_name) << std::endl;
    }

    bool first_load = true;
    if (!reload)
    {
        try
        {
            anim_control_dict_.at(lod_names[0]).at(part_name);
            first_load = false;
        }
        catch (...)
        {
            ;
        }
    }

    if (first_load)
    {
        for (const auto& l_name: lod_names)
        {
            anim_control_dict_.insert({l_name, {}});
            anim_control_dict_.at(l_name).insert({part_name, {}});
        }
    }

    for (const auto& key_val: anims)
    {
        const auto& anim_name = key_val.first;
        const auto& filename = key_val.second;

        // make sure this lod is in anim control dict
        for (const auto& l_name: lod_names)
        {
            auto& part_dict = anim_control_dict_.at(l_name);
            if (first_load)
            {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
                part_dict.at(part_name).insert_or_assign(anim_name, AnimDef());
#else
                part_dict.at(part_name).insert({anim_name, AnimDef()});
#endif
            }

            auto& anim_def = part_dict.at(part_name).at(anim_name);

            if (filename.which() == 1)
            {
                // We were given a pre-load anim bundle, not a filename.
                NodePath anim_bundle_np = boost::get<NodePath>(filename);
                if (anim_bundle_np.is_empty())
                    throw std::runtime_error(fmt::format("NodePath is empty!"));
                if (!anim_bundle_np.node()->is_of_type(AnimBundleNode::get_class_type()))
                {
                    anim_bundle_np.find("**/+AnimBundleNode");
                }
                if (anim_bundle_np.is_empty())
                    throw std::runtime_error(fmt::format("AnimBundleNode is empty!"));
                anim_def.anim_bundle = DCAST(AnimBundleNode, anim_bundle_np.node())->get_bundle();
            }
            else
            {
                // We were given a filename that must be loaded.
                // Store the filename only; we will load and bind
                // it (and produce an AnimControl) when it is
                // played.
                anim_def.filename = boost::get<Filename>(filename);
            }
        }
    }
}

const Actor::PartDef* Actor::get_part_def(const std::string& part_name, const std::string& lod_name) const
{
    const auto& found = part_bundle_dict_.find(lod_name);
    if (found == part_bundle_dict_.end())
    {
        rppanda_actor_cat.warning() << "No lod named: " << lod_name << std::endl;
        return nullptr;
    }

    const auto& part_bundle_dict = found->second;

    std::reference_wrapper<const std::string> true_name(part_name);
    const auto& subpart_dict_iter = subpart_dict_.find(part_name);
    if (subpart_dict_iter != subpart_dict_.end())
        true_name = std::cref(subpart_dict_iter->second.true_part_name);

    const auto& part_def_found = part_bundle_dict.find(true_name);
    if (part_def_found != part_bundle_dict.end())
        return &part_def_found->second;

    return nullptr;
}

NodePath Actor::get_part(const std::string& part_name, const std::string& lod_name) const
{
    if (auto part_def = get_part_def(part_name, lod_name))
        return part_def->part_bundle_np;
    else
        return {};
}

PartBundle* Actor::get_part_bundle(const std::string& part_name, const std::string& lod_name) const
{
    if (auto part_def = get_part_def(part_name, lod_name))
        return part_def->get_bundle();
    else
        return nullptr;
}

void Actor::hide_part(const std::string& part_name, const std::string& lod_name)
{
    auto part = get_part(part_name, lod_name);
    if (part)
        part.hide();
    else
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
}

void Actor::show_part(const std::string& part_name, const std::string& lod_name)
{
    auto part = get_part(part_name, lod_name);
    if (part)
        part.show();
    else
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
}

void Actor::show_all_parts(const std::string& part_name, const std::string& lod_name)
{
    auto part = get_part(part_name, lod_name);
    if (part)
    {
        part.show();
        part.get_children().show();
    }
    else
    {
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
    }
}

NodePath Actor::expose_joint(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name, bool local_transform)
{
    auto part_def = get_part_def(part_name, lod_name);
    if (!part_def)
    {
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
        return {};
    }

    // Get a handle to the joint.
    auto joint = DCAST(CharacterJoint, part_def->get_bundle()->find_child(joint_name));

    if (node.is_empty())
        node = part_def->part_bundle_np.attach_new_node(joint_name);

    if (joint)
    {
        if (local_transform)
            joint->add_local_transform(node.node());
        else
            joint->add_net_transform(node.node());
    }
    else
    {
        rppanda_actor_cat.warning() << "No joint named: " << joint_name << std::endl;
    }

    return node;
}

boost::optional<LMatrix4f> Actor::get_joint_transform(const std::string& part_name, const std::string& joint_name, const std::string& lod_name) const
{
    auto bundle = get_part_bundle(part_name, lod_name);
    if (!bundle)
    {
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
        return boost::none;
    }

    auto joint = bundle->find_child(joint_name);
    if (joint && joint->is_of_type(MovingPartMatrix::get_class_type()))
    {
        return DCAST(MovingPartMatrix, joint)->get_default_value();
    }
    else
    {
        rppanda_actor_cat.warning() << "No joint named: " << joint_name << std::endl;
        return boost::none;
    }
}

CPT(TransformState) Actor::get_joint_transform_state(const std::string& part_name, const std::string& joint_name, const std::string& lod_name) const
{
    auto bundle = get_part_bundle(part_name, lod_name);
    if (!bundle)
    {
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
        return nullptr;
    }

    auto joint = bundle->find_child(joint_name);
    if (joint && joint->is_of_type(CharacterJoint::get_class_type()))
    {
        return DCAST(CharacterJoint, joint)->get_transform_state();
    }
    else
    {
        rppanda_actor_cat.warning() << "No joint named: " << joint_name << std::endl;
        return nullptr;
    }
}

NodePath Actor::control_joint(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name)
{
    auto part_def = get_part_def(part_name, lod_name);
    if (!part_def)
    {
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
        return {};
    }

    PartBundle* bundle = part_def->get_bundle();
    if (node.is_empty())
    {
        // This is not same as original codes.
        // Similar patch (panda3d/panda3d#221) is in exposed_joint function,
        // but it is meaningless in control joint.
        // However, we change this for consistency.
        node = part_def->part_bundle_np.attach_new_node(new ModelNode(joint_name));

        auto joint = bundle->find_child(joint_name);
        if (joint && joint->is_of_type(MovingPartMatrix::get_class_type()))
            node.set_mat(DCAST(MovingPartMatrix, joint)->get_default_value());
    }

    if (!bundle->control_joint(joint_name, node.node()))
        rppanda_actor_cat.warning() << "Cannot control joint " << joint_name << std::endl;

    return node;
}

NodePath Actor::control_joint_tree(NodePath node, const std::string& part_name, const std::string& joint_name, const std::string& lod_name)
{
    auto part_def = get_part_def(part_name, lod_name);
    if (!part_def)
    {
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
        return {};
    }

    auto joint = part_def->get_bundle()->find_child(joint_name);
    if (!joint)
    {
        rppanda_actor_cat.warning() << "Cannot find joint: " << joint_name << std::endl;
        return {};
    }

    if (joint->is_of_type(MovingPartMatrix::get_class_type()))
    {
        if (!node)
        {
            node = part_def->part_bundle_np.attach_new_node(new ModelNode(joint_name));
            node.set_mat(DCAST(MovingPartMatrix, joint)->get_default_value());
        }

        if (!joint->apply_control(node.node()))
            rppanda_actor_cat.warning() << "Cannot control joint " << joint_name << std::endl;

        // build subtree of the joint more faster than Actor::control_joint
        for (int k = 0, k_end = joint->get_num_children(); k < k_end; ++k)
            build_control_joint_tree(joint->get_child(k), node);

        return node;
    }
    else
    {
        rppanda_actor_cat.warning() << "This is not joint: " << joint_name << std::endl;
        return {};
    }
}

NodePath Actor::control_all_joints(NodePath root, const std::string& part_name, const std::string& lod_name)
{
    auto part_def = get_part_def(part_name, lod_name);
    if (!part_def)
    {
        rppanda_actor_cat.warning() << "No part named: " << part_name << std::endl;
        return {};
    }

    if (!root)
        root = part_def->part_bundle_np;

    build_control_joint_tree(part_def->get_bundle(), root);

    return root;
}

void Actor::freeze_joint(const std::string& part_name, const std::string& joint_name, CPT(TransformState) transform,
    const LVecBase3f& pos, const LVecBase3f& hpr, const LVecBase3f& scale)
{
    if (!transform)
        transform = TransformState::make_pos_hpr_scale(pos, hpr, scale);

    std::reference_wrapper<const std::string> true_name(part_name);
    const auto& iter = subpart_dict_.find(part_name);
    if (iter != subpart_dict_.end())
        true_name = std::cref(iter->second.true_part_name);

    bool any_good = false;

    for (const auto& bundle_dict : part_bundle_dict_)
    {
        const auto& part_def = bundle_dict.second.at(true_name);
        if (part_def.get_bundle()->freeze_joint(joint_name, transform))
            any_good = true;
    }

    if (!any_good)
        rppanda_actor_cat.warning() << "Cannot freeze joint " << joint_name << std::endl;
}

void Actor::release_joint(const std::string& part_name, const std::string& joint_name)
{
    std::reference_wrapper<const std::string> true_name(part_name);
    const auto& iter = subpart_dict_.find(part_name);
    if (iter != subpart_dict_.end())
        true_name = std::cref(iter->second.true_part_name);

    for (const auto& bundle_dict : part_bundle_dict_)
    {
        const auto& part_def = bundle_dict.second.at(true_name);
        part_def.get_bundle()->release_joint(joint_name);
    }
}

void Actor::set_control_effect(const std::vector<std::string>& anim_name, float effect,
    const std::vector<std::string>& part_name,
    boost::optional<std::string> lod_name)
{
    for (auto&& control: get_anim_controls(anim_name, part_name, lod_name))
        control->get_part()->set_control_effect(control, effect);
}

void Actor::set_control_effect(bool, float effect,
    const std::vector<std::string>& part_name,
    boost::optional<std::string> lod_name)
{
    for (auto&& control : get_anim_controls(true, part_name, lod_name))
        control->get_part()->set_control_effect(control, effect);
}

void Actor::show_all_bounds()
{
    const auto& geom_nodes = geom_node_.find_all_matches("**/+GeomNode");
    for (int k=0, k_end=geom_nodes.get_num_paths(); k < k_end; ++k)
        geom_nodes.get_path(k).show_bounds();
}

void Actor::hide_all_bounds()
{
    const auto& geom_nodes = geom_node_.find_all_matches("**/+GeomNode");
    for (int k=0, k_end=geom_nodes.get_num_paths(); k < k_end; ++k)
        geom_nodes.get_path(k).hide_bounds();
}

PT(ActorInterval) Actor::actor_interval(const std::vector<std::string>& anim_name, bool loop,
    bool constrained_loop, boost::optional<double> duration,
    boost::optional<double> start_time, boost::optional<double> end_time,
    boost::optional<double> start_frame, boost::optional<double> end_frame,
    double play_rate, const boost::optional<std::string> name, bool force_update,
    const std::vector<std::string>& part_name, const boost::optional<std::string>& lod_name)
{
    return new ActorInterval(this, anim_name, loop, constrained_loop, duration,
        start_time, end_time, start_frame, end_frame, play_rate, name, force_update,
        part_name, lod_name);
}

// ************************************************************************************************

Actor::AnimDef::AnimDef(const std::string& filename, AnimBundle* anim_bundle): filename(filename), anim_bundle(anim_bundle)
{
}

Actor::AnimDef Actor::AnimDef::make_copy()
{
    return AnimDef(filename, anim_bundle);
}

Actor::SubpartDef::SubpartDef(const std::string& true_part_name, const PartSubset& subset):
    true_part_name(true_part_name), subset(subset)
{
}

void Actor::build_LOD_dict_items(std::vector<std::string>& part_name_list, LODDictType::iterator& lod_begin, LODDictType::iterator& lod_end, const boost::optional<std::string>& lod_name)
{
    if (part_name_list.empty() && subparts_complete_)
    {
        // If we have the __subpartsComplete flag, and no partName
        // is specified, it really means to play the animation on
        // all subparts, not on the overall Actor.
        part_name_list.reserve(subpart_dict_.size());
        for (const auto& key_val: subpart_dict_)
            part_name_list.push_back(key_val.first);
    }

    // build list of lodNames and corresponding animControlDicts
    // requested.
    lod_end = anim_control_dict_.end();
    if (!lod_name || this_merge_LOD_bundles_)
    {
        // Get all LOD's
        lod_begin = anim_control_dict_.begin();
    }
    else
    {
        auto found = anim_control_dict_.find(lod_name.value());
        if (found == lod_end)
        {
            rppanda_actor_cat.warning() << "Couldn't find lod: " << lod_name.value() << std::endl;
            lod_begin = lod_end;
        }
        else
        {
            lod_begin = found;
            lod_end = ++found;
        }
    }
}

void Actor::build_anim_dict_items(std::vector<PartDictType::iterator>& anim_dict_items, const std::vector<std::string>& part_name_list, PartDictType& part_dict)
{
    // Now, build the list of partNames and the corresponding
    // animDicts.
    if (part_name_list.empty())
    {
        // Get all main parts, but not sub-parts.
        for (auto part_dict_iter=part_dict.begin(), part_dict_iter_end=part_dict.end(); part_dict_iter != part_dict_iter_end; ++part_dict_iter)
        {
            if (subpart_dict_.find(part_dict_iter->first) == subpart_dict_.end())
                anim_dict_items.push_back(part_dict_iter);
        }
    }
    else
    {
        bool map_changed = false;
        // Get exactly the named part or parts.
        for (const auto& pname: part_name_list)
        {
            auto part_dict_iter = part_dict.find(pname);
            if (part_dict_iter == part_dict.end())
            {
                // Maybe it's a subpart that hasn't been bound yet.
                const auto& subpart_dict_iter = subpart_dict_.find(pname);
                if (subpart_dict_iter != subpart_dict_.end())
                {
                    part_dict_iter = part_dict.insert({ pname,{} }).first;
                    map_changed = true;
                }
            }

            if (part_dict_iter == part_dict.end())
            {
                // part was not present
                rppanda_actor_cat.warning() << "Couldn't find part: " << pname << std::endl;
            }
            else
            {
                anim_dict_items.push_back(part_dict_iter);
            }
        }

        if (map_changed)
        {
            anim_dict_items.clear();
            for (const auto& pname: part_name_list)
            {
                auto part_dict_iter = part_dict.find(pname);
                if (part_dict_iter != part_dict.end())
                    anim_dict_items.push_back(part_dict_iter);
            }
        }
    }
}

bool Actor::build_controls_from_anim_name(std::vector<AnimControl*>& controls, const std::vector<std::string>& names, AnimDictType& anim_dict, PartDictType& part_dict,
    const std::vector<std::string>& part_name_list, const std::string& part_name, const std::string& lod_name, bool allow_async_bind)
{
    for (const auto& anim_name: names)
    {
        auto found = anim_dict.find(anim_name);
        if (found == anim_dict.end() && !part_name_list.empty())
        {
            for (const auto& pname: part_name_list)
            {
                // Maybe it's a subpart that hasn't been bound yet.
                auto subpart_dict_found = subpart_dict_.find(pname);
                if (subpart_dict_found != subpart_dict_.end())
                {
                    const auto& true_part_name = subpart_dict_found->second.true_part_name;
                    auto anim_found = part_dict.at(true_part_name).find(anim_name);
                    if (anim_found != part_dict.at(true_part_name).end())
                    {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
                        found = anim_dict.insert_or_assign(anim_name, anim_found->second.make_copy()).first;
#else
                        auto anim_dict_found = anim_dict.find(anim_name);
                        if (anim_dict_found == anim_dict.end())
                        {
                            found = anim_dict.insert({ anim_name, anim_found->second.make_copy() }).first;
                        }
                        else
                        {
                            anim_dict_found->second = anim_found->second.make_copy();
                            found = anim_dict_found;
                        }
#endif
                    }
                }
            }
        }

        if (found == anim_dict.end())
        {
            // anim was not present
            rppanda_actor_cat.error() << "Couldn't find anim: " << anim_name << std::endl;
            return false;
        }
        else
        {
            // bind the animation first if we need to
            auto& anim = found->second;
            AnimControl* anim_control = anim.anim_control;
            if (!anim_control)
            {
                anim_control = bind_anim_to_part(anim_name, part_name, lod_name, allow_async_bind);
            }
            else if (allow_async_bind)
            {
                // Force the animation to load if it's
                // not already loaded.
                anim_control->wait_pending();
            }

            if (anim_control)
                controls.push_back(anim_control);
        }
    }

    return true;
}

void Actor::do_list_joints(size_t indent_level, const PartGroup* part, bool is_included, const PartSubset& subset) const
{
    const auto& name = part->get_name();
    if (subset.matches_include(name))
        is_included = true;
    else if (subset.matches_exclude(name))
        is_included = false;

    if (is_included)
    {
        std::string value;
        if (part->is_of_type(MovingPartBase::get_class_type()))
        {
            std::stringstream stream;
            DCAST(MovingPartBase, part)->output_value(stream);
            value = stream.str();
        }
        std::cout << fmt::format("{} {} {}", std::string(indent_level, ' '), part->get_name(), value) << std::endl;
    }

    for (int k=0, k_end=part->get_num_children(); k < k_end; ++k)
        do_list_joints(indent_level+2, part->get_child(k), is_included, subset);
}

void Actor::post_load_model(NodePath model, const std::string& part_name, const std::string& lod_name, bool auto_bind_anims)
{
    NodePath bundle_np;
    if (model.node()->is_of_type(Character::get_class_type()))
        bundle_np = model;
    else
        bundle_np = model.find("**/+Character");

    if (bundle_np.is_empty())
    {
        rppanda_actor_cat.warning() << model << " is not a character!" << std::endl;
        model.reparent_to(geom_node_);
    }
    else
    {
        // Maybe the model file also included some animations.  If
        // so, try to bind them immediately and put them into the
        // animControlDict.
        std::shared_ptr<AnimControlCollection> acc;
        int num_anims = 0;
        if (auto_bind_anims)
        {
            acc = std::make_shared<AnimControlCollection>();
            auto_bind(model.node(), *acc, ~0);
            num_anims = acc->get_num_anims();
        }

        //  Now extract out the Character and integrate it with
        // the Actor.

        if (lod_name != Default::lod_name)
        {
            // parent to appropriate node under LOD switch
            bundle_np.reparent_to(LOD_node_.find(lod_name));
        }
        else
        {
            bundle_np.reparent_to(geom_node_);
        }
        prepare_bundle(bundle_np, model.node(), part_name, lod_name);

        // we rename this node to make Actor copying easier
        bundle_np.node()->set_name(fmt::format("{}{}", Actor::part_prefix_, part_name));

        if (num_anims == 0)
            return;

        // If the model had some animations, store them in the
        // dict so they can be played.
        rppanda_actor_cat.info() << "Model contains " << num_anims << " animations." << std::endl;

        // make sure this lod is in anim control dict
        std::string new_lod_name = lod_name;
        if (this_merge_LOD_bundles_)
            new_lod_name = "common";

        anim_control_dict_.insert({ new_lod_name,{} });
        anim_control_dict_.at(new_lod_name).insert({ part_name,{} });

        for (int i = 0; i < num_anims; ++i)
        {
            auto anim_contorl = acc->get_anim(i);
            auto anim_name = acc->get_anim_name(i);

            AnimDef anim_def;
            anim_def.anim_control = anim_contorl;
#if !defined(_MSC_VER) || _MSC_VER >= 1900
            anim_control_dict_.at(new_lod_name).at(part_name).insert_or_assign(anim_name, anim_def);
#else
            anim_control_dict_.at(new_lod_name).at(part_name)[anim_name] = anim_def;
#endif
        }
    }
}

void Actor::prepare_bundle(NodePath bundle_np, PandaNode* part_model, const std::string& part_name, const std::string& lod_name)
{
    if (subpart_dict_.find(part_name) != subpart_dict_.end())
    {
        rppanda_actor_cat.error() << "Subpart (" << part_name << ") already exists" << std::endl;
        return;
    }

    // Rename the node at the top of the hierarchy, if we
    // haven't already, to make it easier to identify this
    // actor in the scene graph.
    if (!got_name_)
    {
        node()->set_name(bundle_np.node()->get_name());
        got_name_ = true;
    }

    if (part_bundle_dict_.find(lod_name) == part_bundle_dict_.end())
    {
        // make a dictionary to store these parts in
        part_bundle_dict_.insert({ lod_name,{} });
        update_sorted_LOD_names();
    }

    auto& bundle_dict = part_bundle_dict_.at(lod_name);

    Character* node = DCAST(Character, bundle_np.node());
    // A model loaded from disk will always have just one bundle.
    if (node->get_num_bundles() != 1)
    {
        rppanda_actor_cat.error() << "The number of bundles is NOT 1" << std::endl;
        return;
    }

    PartBundleHandle* bundle_handle = node->get_bundle_handle(0);

    if (this_merge_LOD_bundles_)
    {
        if (common_bundle_handles_.find(part_name) != common_bundle_handles_.end())
        {
            // We've already got a bundle for this part; merge it.
            auto loaded_bundle_handle = common_bundle_handles_.at(part_name);
            node->merge_bundles(bundle_handle, loaded_bundle_handle);
            bundle_handle = loaded_bundle_handle;
        }
        else
        {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
            common_bundle_handles_.insert_or_assign(part_name, bundle_handle);
#else
            common_bundle_handles_[part_name] = bundle_handle;
#endif
        }
    }

#if !defined(_MSC_VER) || _MSC_VER >= 1900
    bundle_dict.insert_or_assign(part_name, PartDef{ bundle_np, bundle_handle, part_model });
#else
    {
        auto found = bundle_dict.find(part_name);
        if (found == bundle_dict.end())
            bundle_dict.insert({ part_name, PartDef{ bundle_np, bundle_handle, part_model } });
        else
            found->second = PartDef{ bundle_np, bundle_handle, part_model };
    }
#endif
}

void Actor::update_sorted_LOD_names()
{
    static auto sort_key = [](const std::string& x) {
        static const std::unordered_map<char, int> smap ={
            { 'h', 3 },{ 'm', 2 },{ 'l', 1 },{ 'f', 0 }
        };

        if (!std::isdigit(x[0]))
            return smap.at(x[0]);
        else
            return std::stoi(x);
    };

    sorted_LOD_names_.clear();
    sorted_LOD_names_.reserve(part_bundle_dict_.size());
    for (auto&& bundle: part_bundle_dict_)
        sorted_LOD_names_.push_back(bundle.first);

    std::sort(sorted_LOD_names_.begin(), sorted_LOD_names_.end(),
        [](const std::string& lhs, const std::string& rhs) {
        return sort_key(rhs) < sort_key(lhs);
    });
}

AnimControl* Actor::bind_anim_to_part(const std::string& anim_name, const std::string& part_name, const std::string& lod_name, bool allow_async_bind)
{
    // make sure this anim is in the dict
    std::reference_wrapper<const std::string> true_part_name(part_name);
    PartSubset subpart_subset;
    const auto& subpart_dict_iter = subpart_dict_.find(part_name);
    if (subpart_dict_iter != subpart_dict_.end())
    {
        true_part_name = std::cref(subpart_dict_iter->second.true_part_name);
        subpart_subset = subpart_dict_iter->second.subset;
    }

    auto& part_dict = anim_control_dict_[lod_name];
    auto anim_dict_found = part_dict.find(part_name);
    if (anim_dict_found == part_dict.end())
    {
        // It must be a subpart that hasn't been bound yet.
        anim_dict_found = part_dict.insert({part_name, {}}).first;
    }

    auto anim_found = anim_dict_found->second.find(anim_name);
    if (anim_found == anim_dict_found->second.end())
    {
        // It must be a subpart that hasn't been bound yet.
        auto& anim = part_dict[true_part_name].at(anim_name);
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        anim_found = anim_dict_found->second.insert_or_assign(anim_name, anim.make_copy()).first;
#else
        anim_dict_found->second[anim_name] = anim.make_copy();
        anim_found = anim_dict_found->second.find(anim_name);
#endif
    }

    if (anim_found == anim_dict_found->second.end())
        rppanda_actor_cat.error() << "actor has no animation " << anim_name << std::endl;

    auto& anim = anim_found->second;

    // only bind if not already bound!
    if (anim.anim_control)
        return anim.anim_control;

    PartBundle* bundle;
    if (this_merge_LOD_bundles_)
    {
        bundle = common_bundle_handles_.at(true_part_name)->get_bundle();
    }
    else
    {
        bundle = part_bundle_dict_.at(lod_name).at(true_part_name).get_bundle();
    }

    PT(AnimControl) anim_control;
    if (anim.anim_bundle)
    {
        // We already have a bundle; just bind it.
        anim_control = bundle->bind_anim(anim.anim_bundle, -1, subpart_subset);
    }
    else
    {
        // Load and bind the anim.This might be an asynchronous
        // operation that will complete in the background, but if so it
        // will still return a usable AnimControl.
        anim_control = bundle->load_bind_anim(loader_, Filename(anim.filename), -1, subpart_subset, allow_async_bind && this_allow_async_bind_);
    }

    if (!anim_control)
    {
        // Couldn't bind.  (This implies the binding operation was
        // not attempted asynchronously.)
        return nullptr;
    }

    // store the animControl
    anim.anim_control = anim_control;
    rppanda_actor_cat.debug() << "binding anim: " << anim_name << " to part: " << part_name << ", lod: " << lod_name << std::endl;
    return anim_control;
}

}
