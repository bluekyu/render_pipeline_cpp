#include "render_pipeline/rpcore/effect.h"

#include <regex>

#include <shader.h>
#include <filename.h>
#include <virtualFileSystem.h>
#include <graphicsWindow.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>

#include "render_pipeline/rpcore/render_pipeline.h"
#include "render_pipeline/rpcore/globals.h"
#include "render_pipeline/rpcore/loader.h"
#include "render_pipeline/rpcore/stage_manager.h"
#include "render_pipeline/rppanda/showbase/showbase.h"
#include "rplibs/yaml.hpp"

namespace rpcore {

struct Effect::Impl
{
    using InjectionType = std::map<std::string, std::vector<std::string>>;

    static std::string generate_hash(const std::string& filename, const OptionType& options);

public:
    /**
     * Configuration options which can be set per effect instance.These control
     * which features are available in the effect, and which passes to render.
     */
    const static OptionType DEFAULT_OPTIONS;

    /** { Pass ID, Multiview flag } */
    using PassType = std::pair<std::string, bool>;
    const static std::vector<PassType> PASSES;

    static std::map<std::string, std::shared_ptr<Effect>> GLOBAL_CACHE;

    /**
     * Global counter to store the amount of generated effects, used to create
     * a unique id used for writing temporary files.
     */
    static int EFFECT_ID;

public:
    Impl(Effect& self);

    /**
    * Constructs an effect name from a filename, this is used for writing
    * out temporary files.
    */
    std::string convert_filename_to_name(std::string filename);

    /** Internal method to construct the effect from a yaml object. */
    void parse_content(YAML::Node& parsed_yaml);

    /**
    * Parses a fragment template. This just finds the default template
    * for the shader, and redirects that to construct_shader_from_data.
    */
    void parse_shader_template(const PassType& pass_id_multiview, const std::string& stage,
        YAML::Node& data);

    /** Constructs a shader from a given dataset. */
    std::string construct_shader_from_data(const std::string& pass_id, const std::string& stage,
        const std::string& template_src, YAML::Node& data);

    /**
    * Generates a compiled shader object from a given shader
    * source location and code injection definitions.
    */
    std::string process_shader_template(const std::string& template_src,
        const std::string& cache_key, InjectionType& injections);

public:
    Effect& self_;

    int effect_id_ = EFFECT_ID;
    std::string filename_;
    std::string effect_name_;
    std::string effect_hash_;
    OptionType options_;
    std::map<std::string, std::string> generated_shader_paths_;

    std::map<std::string, PT(Shader)> shader_objs_;
};

const Effect::OptionType Effect::Impl::DEFAULT_OPTIONS =
{
    {"render_gbuffer", true},
    {"render_shadow", true},
    {"render_voxelize", true},
    {"render_envmap", true},
    {"render_forward", false},
    {"alpha_testing", true},
    {"normal_mapping", true},
    {"parallax_mapping", false},
};

const std::vector<Effect::Impl::PassType> Effect::Impl::PASSES ={{"gbuffer", true}, {"shadow", false}, {"voxelize", false}, {"envmap", false}, {"forward", true}};

std::map<std::string, std::shared_ptr<Effect>> Effect::Impl::GLOBAL_CACHE;
int Effect::Impl::EFFECT_ID = 0;

std::string Effect::Impl::generate_hash(const std::string& filename, const OptionType& options)
{
    // Set all options which are not present in the dict to its defaults
    OptionType opt = DEFAULT_OPTIONS;
    for (const auto& pair: opt)
    {
        if (options.find(pair.first) != options.end())
            opt[pair.first] = options.at(pair.first);
    }

    // Hash filename, make sure it has the right format and also resolve
    // it to an absolute path, to make sure that relative paths are cached
    // correctly(otherwise, specifying a different path to the same file
    // will cause a cache miss)
    Filename fname = Filename(filename);
    fname.make_absolute();
    const std::string& file_hash = std::to_string(boost::filesystem::hash_value(fname.to_os_specific()));

    // Hash the options, that is, sort the keys to make sure the values
    // are always in the same order, and then convert the flags to strings using
    // '1' for a set flag, and '0' for a unset flag
    std::string options_hash;
    for (const auto& pair: opt)
    {
        options_hash += opt[pair.first] ? "1" : "0";
    }
    return file_hash + "-" + options_hash;
}

Effect::Impl::Impl(Effect& self): self_(self)
{
}

std::string Effect::Impl::convert_filename_to_name(std::string filename)
{
    boost::erase_all(filename, ".yaml");
    boost::erase_all(filename, "effects/");
    boost::replace_all(filename, "/", "_");
    boost::replace_all(filename, "\\", "_");
    boost::replace_all(filename, ".", "_");

    return filename;
}

void Effect::Impl::parse_content(YAML::Node& parsed_yaml)
{
    YAML::Node& vtx_data = parsed_yaml["vertex"];
    YAML::Node& frag_data = parsed_yaml["fragment"];

    for (const auto& pass_id_multiview: PASSES)
    {
        parse_shader_template(pass_id_multiview, "vertex", vtx_data);
        parse_shader_template(pass_id_multiview, "fragment", frag_data);
    }
}

void Effect::Impl::parse_shader_template(const PassType& pass_id_multiview, const std::string& stage, YAML::Node& data)
{
    const std::string& pass_id = pass_id_multiview.first;
    bool stereo_mode = RenderPipeline::get_global_ptr()->get_setting<bool>("pipeline.stereo_mode") && pass_id_multiview.second;

    std::string template_src;
    if (stage == "fragment")
    {
        template_src = "/$$rp/shader/templates/" + pass_id + ".frag.glsl";
    }
    else if (stage == "vertex")
    {
        // Using a shared vertex shader
        if (stereo_mode)
            template_src = "/$$rp/shader/templates/vertex_stereo.vert.glsl";
        else
            template_src = "/$$rp/shader/templates/vertex.vert.glsl";
    }

    const std::string& shader_path = construct_shader_from_data(pass_id, stage, template_src, data);
    generated_shader_paths_[stage + "-" + pass_id] = shader_path;

    // for stereo, add geometry shader except that NVIDIA single pass stereo exists.
    if (stereo_mode && RenderPipeline::get_global_ptr()->get_stage_mgr()->get_defines().at("NVIDIA_STEREO_VIEW") == "0")
    {
        const std::string& shader_path = construct_shader_from_data(pass_id, std::string("geometry"), "/$$rp/shader/templates/vertex_stereo.geom.glsl", YAML::Node());
        generated_shader_paths_[std::string("geometry-") + pass_id] = shader_path;
    }
}

std::string Effect::Impl::construct_shader_from_data(const std::string& pass_id, const std::string& stage,
    const std::string& template_src, YAML::Node& data)
{
    InjectionType injects ={{std::string("defines"), {}}};

    for (const auto& key_val: options_)
    {
        const std::string& val_str = key_val.second ? "1" : "0";

        injects["defines"].push_back(std::string("#define OPT_" +
            boost::algorithm::to_upper_copy(key_val.first) + " " + val_str));
    }

    injects["defines"].push_back(std::string("#define IN_") + boost::algorithm::to_upper_copy(stage) + "_SHADER 1");
    injects["defines"].push_back(std::string("#define IN_") + boost::algorithm::to_upper_copy(pass_id) + "_SHADER 1");
    injects["defines"].push_back(std::string("#define IN_RENDERING_PASS 1"));

    // Parse dependencies
    if (data.IsDefined() && data["dependencies"])
    {
        for (const auto& dependency: data["dependencies"])
        {
            const std::string& include_str = std::string("#pragma include \"") + dependency.as<std::string>() + "\"";
            injects["includes"].push_back(include_str);
        }
        data.remove("dependencies");
    }

    // Append aditional injects
    for (const auto& node: data)
    {
        const std::string key(node.first.as<std::string>());

        if (node.second.IsNull())
        {
            self_.warn(std::string("Empty insertion: '") + key + "'");
            continue;
        }

        const std::string val(node.second.as<std::string>());
        if (!node.second.IsScalar())
        {
            self_.warn("Invalid syntax, you used a list but you should have used a string:");
            self_.warn(val);
            continue;
        }

        std::regex newline_re("\\n");
        std::vector<std::string> parsed_val(std::sregex_token_iterator(val.begin(), val.end(), newline_re, -1), std::sregex_token_iterator());
        injects[key].insert(injects[key].end(), parsed_val.begin(), parsed_val.end());
    }

    const std::string& cache_key = effect_name_ + "@" + stage + "-" + pass_id + "@" + effect_hash_;
    return process_shader_template(template_src, cache_key, injects);
}

std::string Effect::Impl::process_shader_template(const std::string& template_src, const std::string& cache_key,
    InjectionType& injections)
{
    std::vector<std::string> shader_lines;

    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
    try
    {
        std::istream* file = vfs->open_read_file(template_src, true);

        do
        {
            shader_lines.push_back("");
        } while (std::getline(*file, shader_lines.back()));

        vfs->close_read_file(file);
    }
    catch (const std::exception& err)
    {
        self_.error(std::string("Error reading shader template: ") + err.what());
    }

    std::vector<std::string> parsed_lines ={std::string("\n\n")};
    parsed_lines.push_back("/* Compiled Shader Template");
    parsed_lines.push_back(std::string(" * generated from: '") + template_src + "'");
    parsed_lines.push_back(std::string(" * cache key: '") + cache_key + "'");
    parsed_lines.push_back(" *");
    parsed_lines.push_back(" * !!! Autogenerated, do not edit! Your changes will be lost. !!!");
    parsed_lines.push_back(" */\n\n");

    // Store whether we are in the main function already - we need this
    // to properly insert scoped code blocks
    bool in_main = false;

    for (const auto& line: shader_lines)
    {
        const std::string& stripped_line = boost::to_lower_copy(boost::trim_copy(line));

        // Check if we are already in the main function
        if (stripped_line.find("void main()") != std::string::npos)
            in_main = true;

        // Check if the current line is a hook
        if (stripped_line.front() == '%' && stripped_line.back() == '%')
        {
            // If the line is a hook, get the hook name and save the
            // indent so we can indent all injected lines properly.
            const std::string& hook_name = stripped_line.substr(1, stripped_line.size()-2);
            const std::string indent(line.size() - boost::trim_left_copy(line).size(), ' ');

            // Inject all registered template values into the hook
            if (injections.find(hook_name) != injections.end())
            {
                // Directly remove the value from the list so we can check which
                // hooks were not found in the template
                auto insertions = std::move(injections.at(hook_name));
                injections.erase(hook_name);

                if (insertions.size() > 0)
                {
                    // When we are in the main function, we have to make sure we
                    // use a seperate scope, so there are no conflicts with variable
                    // declarations
                    const std::string& header = indent + "/* Hook: " + hook_name + " */" + (in_main ? " {" : "");
                    parsed_lines.push_back(header);

                    for (const auto& line_to_insert: insertions)
                    {
                        if (line_to_insert.empty())
                        {
                            self_.warn(std::string("Empty insertion a hook '") + hook_name + "'");
                            continue;
                        }

                        // Dont indent defines and pragmas
                        if (line_to_insert[0] == '#')
                            parsed_lines.push_back(line_to_insert);
                        else
                            parsed_lines.push_back(indent + line_to_insert);
                    }

                    if (in_main)
                        parsed_lines.push_back(indent + "}");
                }
            }
        }
        else
        {
            parsed_lines.push_back(boost::trim_right_copy(line));
        }
    }

    // Add a closing newline to the file
    // (C++: add newline when the file is written.)
    //parsed_lines.push_back("");

    // Warn the user about all unused hooks
    for (const auto& key_val: injections)
        self_.warn(std::string("Hook '") + key_val.first + "' not found in template '" + template_src + "'!");

    // Write the constructed shader and load it back
    const std::string& temp_path = std::string("/$$rptemp/$$effect-") + cache_key + ".glsl";

    try
    {
        std::ostream* file = vfs->open_write_file(temp_path, false, false);

        for (const auto& shader_content: parsed_lines)
            *file << shader_content << std::endl;

        vfs->close_write_file(file);
    }
    catch (const std::exception& err)
    {
        self_.error(std::string("Error writing processed shader: ") + err.what());
    }

    return temp_path;
}

// ************************************************************************************************
std::shared_ptr<Effect> Effect::load(const std::string& filename, const OptionType& options)
{
	const std::string& effect_hash = Impl::generate_hash(filename, options);
    if (Impl::GLOBAL_CACHE.find(effect_hash) != Impl::GLOBAL_CACHE.end())
        return Impl::GLOBAL_CACHE[effect_hash];

	auto effect = std::make_shared<Effect>();
	effect->set_options(options);
	if (!effect->do_load(filename))
	{
		RPObject::global_error("Effect", "Could not load effect!");
		return nullptr;
	}

	return effect;
}

Effect::Effect(void): RPObject("Effect"), impl_(std::make_unique<Impl>(*this))
{
    Impl::EFFECT_ID += 1;
    impl_->options_ = Impl::DEFAULT_OPTIONS;
}

Effect::~Effect(void)
{
}

int Effect::get_effect_id(void) const
{
    return impl_->effect_id_;
}

bool Effect::get_option(const std::string& name) const
{
    return impl_->options_.at(name);
}

void Effect::set_options(const OptionType& options)
{
	for (const auto& pair: options)
	{
        if (impl_->options_.find(pair.first) == impl_->options_.end())
		{
			error("Unknown option: " + pair.first);
			continue;
		}
        impl_->options_[pair.first] = pair.second;
	}
}

bool Effect::do_load(const std::string& filename)
{
    impl_->filename_ = filename;
    impl_->effect_name_ = impl_->convert_filename_to_name(filename);
    impl_->effect_hash_ = impl_->generate_hash(filename, impl_->options_);

    // Load the YAML file
    YAML::Node parsed_yaml;
    if (!rplibs::load_yaml_file(filename, parsed_yaml))
        return false;
    impl_->parse_content(parsed_yaml);

    // Construct a shader object for each pass
    for (const auto& pass_id_multiview: Impl::PASSES)
    {
        const std::string& vertex_src = impl_->generated_shader_paths_.at("vertex-" + pass_id_multiview.first);
        const std::string& fragment_src = impl_->generated_shader_paths_.at("fragment-" + pass_id_multiview.first);
        std::string geometry_src;

        if (impl_->generated_shader_paths_.find("geometry-" + pass_id_multiview.first) != impl_->generated_shader_paths_.end())
            geometry_src = impl_->generated_shader_paths_.at("geometry-" + pass_id_multiview.first);

        if (geometry_src.empty())
            impl_->shader_objs_[pass_id_multiview.first] = RPLoader::load_shader({vertex_src, fragment_src});
        else
            impl_->shader_objs_[pass_id_multiview.first] = RPLoader::load_shader({vertex_src, fragment_src, geometry_src});
    }

    return true;
}

Shader* Effect::get_shader_obj(const std::string& pass_id) const
{
	try
	{
		return impl_->shader_objs_.at(pass_id);
	}
	catch (const std::out_of_range&)
	{
		warn(std::string("Pass '") + pass_id + "' not found!");
		return nullptr;
	}
}

}
