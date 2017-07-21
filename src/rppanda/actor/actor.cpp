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
#include <animBundleNode.h>

#include <spdlog/fmt/ostr.h>

#include "rppanda/config_rppanda.hpp"

namespace rppanda {

class Actor::Impl
{
public:
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
        std::string true_part_name;
        PartSubset subset;
    };

public:
    void initialize(Actor* self,
        const boost::variant<void*, ModelsType, LODModelsType, MultiPartLODModelsType>& models,
        const boost::variant<void*, AnimsType, MultiPartAnimsType>& anims,
        boost::optional<NodePath> other,
        bool copy,
        bool flattenable,
        bool set_final,
        boost::optional<bool> merge_LOD_bundles,
        boost::optional<bool> allow_async_bind,
        boost::optional<bool> ok_missing);

    void post_load_model(Actor* self, NodePath model, const std::string& part_name, const std::string& lod_name, bool auto_bind_anims);
    void prepare_bundle(Actor* self, NodePath bundle_np, PandaNode* part_model, const std::string& part_name="modelRoot", const std::string& lod_name="lodRoot");

    /**
     * Cache the sorted LOD names so we don't have to grab them
     * and sort them every time somebody asks for the list.
     */
    void update_sorted_LOD_names(void);

public:
    Loader* loader_;

    bool merge_LOD_bundles_;
    bool allow_async_bind_;

    std::unordered_map<std::string, SubpartDef> subpart_dict_;
    std::unordered_map<std::string,
        std::unordered_map<std::string,
        std::unordered_map<std::string, AnimDef>>> anim_control_dict_;
    std::unordered_map<std::string, std::unordered_map<std::string, PartDef>> part_bundle_dict_;
    std::unordered_map<std::string, PT(PartBundleHandle)> common_bundle_handles_;

    std::vector<std::string> sorted_LOD_names_;

    bool got_name_;

    NodePath geom_node_;
    NodePath lod_node_;
};

void Actor::Impl::initialize(Actor* self,
    const boost::variant<void*, ModelsType, LODModelsType, MultiPartLODModelsType>& models,
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
        merge_LOD_bundles_ = merge_LOD_bundles.get();
    else
        merge_LOD_bundles_ = Actor::merge_LOD_bundles.get_value();

    // Set the allowAsyncBind flag.  If this is true, it enables
    // asynchronous animation binding.  This requires that you have
    // run "egg-optchar -preload" on your animation and models to
    // generate the appropriate AnimPreloadTable.

    if (allow_async_bind)
        allow_async_bind_ = allow_async_bind.get();
    else
        allow_async_bind_ = Actor::allow_async_bind.get_value();

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
            self->NodePath::operator=(std::move(NodePath(root)));
            self->set_geom_node(*self);
        }
        else
        {
            // A standard Actor has a ModelNode at the root, and
            // another ModelNode to protect the GeomNode.
            PT(ModelNode) model = new ModelNode("actor");
            root = model;

            model->set_preserve_transform(ModelNode::PreserveTransform::PT_local);
            self->NodePath::operator=(std::move(NodePath(root)));
            self->set_geom_node(self->attach_new_node(new ModelNode("actorGeom")));
        }

        // do we have a map of models?
        if (models.which() == 2)
        {
            // TODO
        }
        // else it is a single part actor
        else if (models.which() == 1)
        {
            self->load_model(boost::get<ModelsType>(models), "modelRoot", "lodRoot", copy, ok_missing);
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
                        self->load_anims(partname_anims.second, partname_anims.first, lod_name);
                }
            }
            else if (models.which() == 1)
            {
                // then it must be multi-part w/o LOD
                for (const auto& partname_anims: multipart_anims)
                    self->load_anims(partname_anims.second, partname_anims.first);
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
                self->load_anims(boost::get<AnimsType>(anims), "modelRoot", lod_name);
        }
        // else it is single-part w/o LOD
        else if (anims.which() == 1)
        {
            self->load_anims(boost::get<AnimsType>(anims));
        }
    }
    else
    {
        // TODO
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

void Actor::Impl::post_load_model(Actor* self, NodePath model, const std::string& part_name, const std::string& lod_name, bool auto_bind_anims)
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
        prepare_bundle(self, bundle_np, model.node(), part_name, lod_name);

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

        anim_control_dict_.insert({new_lod_name, {}});
        anim_control_dict_.at(new_lod_name).insert({part_name, {}});

        for (int i = 0; i < num_anims; ++i)
        {
            auto anim_contorl = acc->get_anim(i);
            auto anim_name = acc->get_anim_name(i);

            Impl::AnimDef anim_def;
            anim_def.anim_control = anim_contorl;
#if _MSC_VER >= 1900
            anim_control_dict_.at(new_lod_name).at(part_name).insert_or_assign(anim_name, anim_def);
#else
            anim_control_dict_.at(new_lod_name).at(part_name).insert({anim_name, anim_def});
#endif
        }
    }
}

void Actor::Impl::prepare_bundle(Actor* self, NodePath bundle_np, PandaNode* part_model, const std::string& part_name, const std::string& lod_name)
{
    if (subpart_dict_.find(part_name) != subpart_dict_.end())
        throw std::runtime_error("Subpart (" + part_name + ") already exists");

    // Rename the node at the top of the hierarchy, if we
    // haven't already, to make it easier to identify this
    // actor in the scene graph.
    if (!got_name_)
    {
        self->node()->set_name(bundle_np.node()->get_name());
        got_name_ = true;
    }

    if (part_bundle_dict_.find(lod_name) == part_bundle_dict_.end())
    {
        // make a dictionary to store these parts in
        part_bundle_dict_.insert({lod_name, {}});
        update_sorted_LOD_names();
    }

    auto& bundle_dict = part_bundle_dict_.at(lod_name);

    Character* node = DCAST(Character, bundle_np.node());
    // A model loaded from disk will always have just one bundle.
    if (node->get_num_bundles() != 1)
        throw std::runtime_error("The number of bundles is NOT 1");

    PT(PartBundleHandle) bundle_handle = node->get_bundle_handle(0);

    if (merge_LOD_bundles_)
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
#if _MSC_VER >= 1900
            common_bundle_handles_.insert_or_assign(part_name, bundle_handle);
#else
            common_bundle_handles_.insert({part_name, bundle_handle});
#endif
        }
    }

#if _MSC_VER >= 1900
    bundle_dict.insert_or_assign(part_name, Impl::PartDef{bundle_np, bundle_handle, part_model});
#else
    bundle_dict.insert({part_name, Impl::PartDef{bundle_np, bundle_handle, part_model}});
#endif
}

void Actor::Impl::update_sorted_LOD_names(void)
{
    static auto sort_key = [](const std::string& x) {
        static const std::unordered_map<char, int> smap = {
            {'h', 3}, {'m', 2}, {'l', 1}, {'f', 0}
        };

        if (!std::isdigit(x[0]))
            return smap.at(x[0]);
        else
            return std::stoi(x);
    };

    sorted_LOD_names_.clear();
    sorted_LOD_names_.reserve(part_bundle_dict_.size());
    for (auto& bundle: part_bundle_dict_)
        sorted_LOD_names_.push_back(bundle.first);

    std::sort(sorted_LOD_names_.begin(), sorted_LOD_names_.end(),
        [](const std::string& lhs, const std::string& rhs){
        return sort_key(rhs) < sort_key(lhs);
    });
}

// ************************************************************************************************

std::string Actor::part_prefix("__Actor_");

LoaderOptions Actor::model_loader_options(LoaderOptions::LF_search | LoaderOptions::LF_report_errors || LoaderOptions::LF_convert_skeleton);
LoaderOptions Actor::anim_loader_options(LoaderOptions::LF_search | LoaderOptions::LF_report_errors || LoaderOptions::LF_convert_anim);

ConfigVariableBool Actor::validate_subparts("validate-subparts", true);
ConfigVariableBool Actor::merge_LOD_bundles("merge-lod-bundles", true);
ConfigVariableBool Actor::allow_async_bind("allow-async-bind", true);

Actor::Actor(const boost::variant<void*, ModelsType, LODModelsType, MultiPartLODModelsType>& models,
    const boost::variant<void*, AnimsType, MultiPartAnimsType>& anims,
    boost::optional<NodePath> other,
    bool copy,
    bool flattenable,
    bool set_final,
    boost::optional<bool> merge_LOD_bundles,
    boost::optional<bool> allow_async_bind,
    boost::optional<bool> ok_missing): impl_(std::make_unique<Impl>())
{
    impl_->initialize(this, models, anims, other, copy, flattenable, set_final, merge_LOD_bundles, allow_async_bind, ok_missing);
}

Actor::~Actor(void) = default;

Actor& Actor::operator=(Actor&&) = default;

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

    rppanda_cat.debug() << fmt::format("in load_model: {}, part: {}, lod: {}, copy: {}", model_path, part_name, lod_name, copy) << std::endl;

    NodePath model;
    if (copy)
        model = model_path.copy_to(NodePath());
    else
        model = model_path;

    impl_->post_load_model(this, model, part_name, lod_name, auto_bind_anims);
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

    impl_->post_load_model(this, model, part_name, lod_name, auto_bind_anims);
}

void Actor::load_anims(const AnimsType& anims, const std::string& part_name, const std::string& lod_name)
{
    bool reload = true;
    std::vector<std::string> lod_names;
    if (merge_LOD_bundles)
    {
        lod_names.push_back("common");
    }
    else if (lod_name == "all")
    {
        reload = false;
        // TODO
    }
    else
    {
        lod_names.push_back(lod_name);
    }

    if (rppanda_cat.is_on(NotifySeverity::NS_debug))
    {
        std::stringstream ss;
        ss << "{";
        for (const auto& key_val: anims)
            ss << key_val.first << ":" << key_val.second << ", ";
        ss << "}";

        rppanda_cat.debug() << fmt::format("in load_anims: {}, part: {}, lod: {}", ss.str(), part_name, lod_name) << std::endl;
    }

    bool first_load = true;
    if (!reload)
    {
        try
        {
            impl_->anim_control_dict_.at(lod_names[0]).at(part_name);
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
            impl_->anim_control_dict_.insert({l_name, {}});
            impl_->anim_control_dict_.at(l_name).insert({part_name, {}});
        }
    }

    for (const auto& key_val: anims)
    {
        const auto& anim_name = key_val.first;
        const auto& filename = key_val.second;

        // make sure this lod is in anim control dict
        for (const auto& l_name: lod_names)
        {
            auto& part_dict = impl_->anim_control_dict_.at(l_name);
            if (first_load)
            {
#if _MSC_VER >= 1900
                part_dict.at(part_name).insert_or_assign(anim_name, Impl::AnimDef());
#else
                part_dict.at(part_name).insert({anim_name, Impl::AnimDef()});
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

}
