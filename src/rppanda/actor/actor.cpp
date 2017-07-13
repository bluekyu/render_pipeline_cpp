/**
 * This is C++ porting codes of direct/src/actor/Actor.py
 */

#include "render_pipeline/rppanda/actor/actor.hpp"

#include <loader.h>
#include <modelNode.h>
#include <partSubset.h>
#include <character.h>
#include <animControlCollection.h>
#include <auto_bind.h>

#include <spdlog/fmt/ostr.h>

#include "rppanda/config_rppanda.hpp"

namespace rppanda {

class Actor::Impl
{
public:
    class AnimDef
    {
    public:
        std::string filename;
        PT(AnimControl) anim_control = nullptr;
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
        std::string true_part_name;
        PartSubset subset;
    };

public:
    Impl(Actor& self, const Parameters& params);

    void post_load_model(NodePath model, const std::string& part_name, const std::string& lod_name, bool auto_bind_anims);
    void prepare_bundle(NodePath bundle_np, PandaNode* part_model, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot");

public:
    Actor& self_;

    Loader* loader_;

    bool merge_LOD_bundles_;
    bool allow_async_bind_;

    std::unordered_map<std::string, SubpartDef> subpart_dict_;
    std::unordered_map<std::string,
        std::unordered_map<std::string,
        std::unordered_map<std::string, AnimDef>>> anim_control_dict_;

    bool got_name_;

    NodePath geom_node_;
    NodePath lod_node_;
};

Actor::Impl::Impl(Actor& self, const Parameters& params): self_(self)
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

    if (params.merge_LOD_bundles)
        merge_LOD_bundles_ = params.merge_LOD_bundles.get();
    else
        merge_LOD_bundles_ = Actor::merge_LOD_bundles.get_value();

    // Set the allowAsyncBind flag.  If this is true, it enables
    // asynchronous animation binding.  This requires that you have
    // run "egg-optchar -preload" on your animation and models to
    // generate the appropriate AnimPreloadTable.

    if (params.allow_async_bind)
        allow_async_bind_ = params.allow_async_bind.get();
    else
        allow_async_bind_ = Actor::allow_async_bind.get_value();

    // act like a normal constructor

    // create base hierarchy
    got_name_ = false;

    PT(PandaNode) root;
    if (params.flattenable)
    {
        // If we want a flattenable Actor, don't create all
        // those ModelNodes, and the GeomNode is the same as
        // the root.
        root = new PandaNode("actor");
        self_.NodePath::operator=(NodePath(root));
        self_.set_geom_node(NodePath(self_));
    }
    else
    {
        // A standard Actor has a ModelNode at the root, and
        // another ModelNode to protect the GeomNode.
        PT(ModelNode) model = new ModelNode("actor");
        root = model;

        model->set_preserve_transform(ModelNode::PreserveTransform::PT_local);
        self_.NodePath::operator=(NodePath(root));
        self_.set_geom_node(self_.attach_new_node(new ModelNode("actorGeom")));
    }

    if (params.models)
    {
        const auto& models = params.models.get();

        // do we have a map of models?
        if (models.which() == 1)
        {
            // TODO
        }
        else
        {
            // else it is a single part actor
            self_.load_model(boost::get<Filename>(models), "modelRoot", "lodRoot", params.copy, params.ok_missing);
        }

        // TODO
    }
}

void Actor::Impl::post_load_model(NodePath model, const std::string& part_name, const std::string& lod_name, bool auto_bind_anims)
{
    if (model.is_empty())
        throw std::runtime_error(fmt::format("Could not load Actor model {}", model));

    NodePath bundle_np;
    if (model.node()->is_of_type(Character::get_class_type()))
        bundle_np = model;
    else
        bundle_np = model.find("**/+Character");

    if (bundle_np.is_empty())
    {
        rppanda_cat.warning() << model << " is not a character!" << std::endl;
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

        if (lod_name != "lodRoot")
        {
            // parent to appropriate node under LOD switch
            bundle_np.reparent_to(lod_node_.find(lod_name));
        }
        else
        {
            bundle_np.reparent_to(geom_node_);
        }
        prepare_bundle(bundle_np, model.node(), part_name, lod_name);

        // we rename this node to make Actor copying easier
        bundle_np.node()->set_name(fmt::format("{}{}", Actor::part_prefix, part_name));

        if (num_anims == 0)
            return;

        // If the model had some animations, store them in the
        // dict so they can be played.
        rppanda_cat.info() << "Model contains " << num_anims << " animations." << std::endl;

        // make sure this lod is in anim control dict
        std::string new_lod_name = lod_name;
        if (merge_LOD_bundles)
            new_lod_name = "common";

#if _MSC_VER >= 1900
        anim_control_dict_.insert_or_assign(new_lod_name, {});
        anim_control_dict_.at(new_lod_name).insert_or_assign(part_name, {});

        for (int i = 0; i < num_anims; ++i)
        {
            auto anim_contorl = acc->get_anim(i);
            auto anim_name = acc->get_anim_name(i);

            Impl::AnimDef anim_def;
            anim_def.anim_control = anim_contorl;
            anim_control_dict_.at(new_lod_name).at(part_name).insert_or_assign(anim_name, anim_def);
        }
#else
        anim_control_dict_.insert({new_lod_name, {}});
        anim_control_dict_.at(new_lod_name).insert({part_name, {}});

        for (int i = 0; i < num_anims; ++i)
        {
            auto anim_contorl = acc->get_anim(i);
            auto anim_name = acc->get_anim_name(i);

            Impl::AnimDef anim_def;
            anim_def.anim_control = anim_contorl;
            anim_control_dict_.at(new_lod_name).at(part_name).insert({anim_name, anim_def});
        }
#endif
    }
}

void Actor::Impl::prepare_bundle(NodePath bundle_np, PandaNode* part_model, const std::string& part_name, const std::string& lod_name)
{
    // TODO
}

// ************************************************************************************************

std::string Actor::part_prefix("__Actor_");

LoaderOptions Actor::model_loader_options(LoaderOptions::LF_search | LoaderOptions::LF_report_errors || LoaderOptions::LF_convert_skeleton);
LoaderOptions Actor::anim_loader_options(LoaderOptions::LF_search | LoaderOptions::LF_report_errors || LoaderOptions::LF_convert_anim);

ConfigVariableBool Actor::validate_subparts("validate-subparts", true);
ConfigVariableBool Actor::merge_LOD_bundles("merge-lod-bundles", true);
ConfigVariableBool Actor::allow_async_bind("allow-async-bind", true);

Actor::Actor(const Parameters& params): impl_(std::make_unique<Impl>(*this, params))
{
}

bool Actor::has_LOD(void) const
{
    return !impl_->lod_node_.is_empty();
}

void Actor::set_geom_node(NodePath node)
{
    impl_->geom_node_ = node;
}

void Actor::load_model(NodePath model_path, const std::string& part_name, const std::string& lod_name,
    bool copy, bool auto_bind_anims)
{
    if (impl_->subpart_dict_.find(part_name) != impl_->subpart_dict_.end())
    {
        rppanda_cat.error() << "Part (" << part_name << ") is already loaded." << std::endl;;
        return;
    }

    rppanda_cat.debug() << fmt::format("in load_model: {}, part: {}, lod: {}, copy: {}", model_path, part_name, lod_name, copy) << std::endl;;

    NodePath model;
    if (copy)
        model = model_path.copy_to(NodePath());
    else
        model = model_path;

    impl_->post_load_model(model, part_name, lod_name, auto_bind_anims);
}

void Actor::load_model(const Filename& model_path, const std::string& part_name, const std::string& lod_name,
    bool copy, const boost::optional<bool>& ok_missing, bool auto_bind_anims)
{
    if (impl_->subpart_dict_.find(part_name) != impl_->subpart_dict_.end())
    {
        rppanda_cat.error() << "Part (" << part_name << ") is already loaded." << std::endl;;
        return;
    }

    rppanda_cat.debug() << fmt::format("in load_model: {}, part: {}, lod: {}, copy: {}", model_path, part_name, lod_name, copy) << std::endl;;

#if _MSC_VER >= 1900
    std::shared_ptr<LoaderOptions> loader_options = std::shared_ptr<LoaderOptions>(&Actor::model_loader_options, [](auto){});
#else
    std::shared_ptr<LoaderOptions> loader_options = std::shared_ptr<LoaderOptions>(&Actor::model_loader_options, [](LoaderOptions*){});
#endif
    if (!copy)
    {
        // If copy = 0, then we should always hit the disk.
        loader_options = std::make_shared<LoaderOptions>(Actor::model_loader_options);
        loader_options->set_flags(loader_options->get_flags() & ~LoaderOptions::LF_no_ram_cache);
    }

    if (ok_missing)
    {
        if (ok_missing.get())
            loader_options->set_flags(loader_options->get_flags() & ~LoaderOptions::LF_report_errors);
        else
            loader_options->set_flags(loader_options->get_flags() | LoaderOptions::LF_report_errors);
    }

    // Pass loaderOptions to specify that we want to
    // get the skeleton model.  This only matters to model
    // files (like .mb) for which we can choose to extract
    // either the skeleton or animation, or neither.
    PT(PandaNode) model_node = impl_->loader_->load_sync(model_path, *loader_options);
    NodePath model;
    if (model_node)
        model = NodePath(model_node);

    impl_->post_load_model(model, part_name, lod_name, auto_bind_anims);
}

}
