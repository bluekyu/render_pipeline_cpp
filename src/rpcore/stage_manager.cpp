/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "render_pipeline/rpcore/stage_manager.hpp"

#include <regex>

#include <boost/algorithm/string.hpp>

#include <virtualFileSystem.h>
#include <shaderInput.h>
#include <texture.h>

#include <spdlog/fmt/fmt.h>

#include "rplibs/yaml.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rpcore/stages/update_previous_pipes_stage.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_stage.hpp"
#include "render_pipeline/rpcore/util/shader_input_blocks.hpp"

namespace rpcore {

class StageManager::Impl
{
public:
    Impl(StageManager& self, RenderPipeline& pipeline);

    /** Loads the order of all stages from the stages.yaml configuration file. */
    void load_stage_order();

    void prepare_stages();

    /** Sets all required pipes on a stage. */
    bool bind_pipes_to_stage(const std::shared_ptr<RenderStage>& stage);

    /** Binds all inputs including common inputs to the given stage. */
    bool bind_inputs_to_stage(const std::shared_ptr<RenderStage>& stage);

    /**
     * Registers all produced pipes, inputs and defines from the given
     * stage, so they can be used by later stages.
     */
    void register_stage_result(const std::shared_ptr<RenderStage>& stage);

    /**
     * Creates a target for each last-frame's pipe, any pipe starting
     * with the prefix 'Previous::' has to be stored and copied each frame.
     */
    bool create_previous_pipes();

    /**
     * Applies all future bindings. At this point all pipes and
     * inputs should be present.
     */
    void apply_future_bindings();

public:
    StageManager& self_;
    RenderPipeline& pipeline_;

    bool created_ = false;
    std::vector<std::shared_ptr<RenderStage>> stages_;
    std::unordered_map<std::string, ShaderInput> inputs_;

    std::unordered_map<std::string, ShaderInput> pipes_;

    /** { id, std::shared_ptr<[SimpleInputBlock | GroupedInputBlock]> } */
    std::unordered_map<std::string, boost::variant<
        std::shared_ptr<SimpleInputBlock>,
        std::shared_ptr<GroupedInputBlock>>> input_blocks_;

    std::vector<boost::variant<
        std::shared_ptr<SimpleInputBlock>,
        std::shared_ptr<GroupedInputBlock>>> input_block_list_;
    std::unordered_map<std::string, std::shared_ptr<Image>> previous_pipes_;

    std::vector<std::pair<std::string, std::shared_ptr<RenderStage>>> future_bindings_;
    DefinesType defines_;

    std::shared_ptr<UpdatePreviousPipesStage> prev_stage_;

    std::vector<std::string> stage_order_;
};

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#else
struct VisitorBindToStage: public boost::static_visitor<>
{
    template <class T>
    void operator()(const T& block)
    {
        block->bind_to(stage);
    }

    std::shared_ptr<RenderStage> stage;
};

struct VisitorInsertToInputBlocks: public boost::static_visitor<>
{
    VisitorInsertToInputBlocks(std::unordered_map<std::string, boost::variant<
        std::shared_ptr<SimpleInputBlock>,
        std::shared_ptr<GroupedInputBlock>>>& input_blocks): input_blocks_(input_blocks)
    {
    }

    template <class T>
    void operator()(const T& block)
    {
        std::string block_name = block->get_name();
        input_blocks_.insert({block_name, block});
    }

private:
    std::unordered_map<std::string, boost::variant<
        std::shared_ptr<SimpleInputBlock>,
        std::shared_ptr<GroupedInputBlock>>>& input_blocks_;
};
#endif

StageManager::Impl::Impl(StageManager& self, RenderPipeline& pipeline): self_(self), pipeline_(pipeline)
{
}

void StageManager::Impl::load_stage_order()
{
    YAML::Node orders;
    if (!rplibs::load_yaml_file("/$$rpconfig/stages.yaml", orders))
        return;

    if (!orders["global_stage_order"].IsDefined())
    {
        self_.error("Could not load stage order, root key does not exist!");
        return;
    }

    const size_t stage_count = orders["global_stage_order"].size();
    stage_order_.reserve(stage_count);
    const std::regex double_colon_re("::");
    for (auto stage_id: orders["global_stage_order"])
        stage_order_.push_back(stage_id.as<std::string>());
}

void StageManager::Impl::prepare_stages()
{
    self_.debug("Preparing stages ..");

    // Remove all disabled stages
    std::vector<std::shared_ptr<RenderStage>> enabled_stages;
    for (auto& stage: stages_)
    {
        if (!stage->disabled_)
            enabled_stages.push_back(stage);
    }

    stages_ = std::move(enabled_stages);

    std::sort(stages_.begin(), stages_.end(), [this](const std::shared_ptr<RenderStage>& lhs, const std::shared_ptr<RenderStage>& rhs) {
        return std::find(stage_order_.begin(), stage_order_.end(), lhs->get_stage_id()) < std::find(stage_order_.begin(), stage_order_.end(), rhs->get_stage_id());
    });
}

bool StageManager::Impl::bind_pipes_to_stage(const std::shared_ptr<RenderStage>& stage)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
#else
    VisitorBindToStage vbts;
    vbts.stage = stage;
#endif

    const auto& required_pipes = stage->get_required_pipes();
    for (const auto& pipe: required_pipes)
    {
        // Check if there is an input block named like the pipe
        if (input_blocks_.find(pipe) != input_blocks_.end())
        {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
            boost::apply_visitor([&](const auto& block) { block->bind_to(stage); }, input_blocks_.at(pipe));
#else
            boost::apply_visitor(vbts, input_blocks_.at(pipe));
#endif
            continue;
        }

        if (pipe.find("PreviousFrame::") == 0)
        {
            // Special case: Pipes from the previous frame.We assume those
            // pipes have the same size as the window and a format of
            // F_rgba16.Could be subject to change.
            const std::regex double_colon_re("::");
            std::vector<std::string> result(std::sregex_token_iterator(pipe.begin(), pipe.end(), double_colon_re, -1), std::sregex_token_iterator());
            const std::string& pipe_name = result.back();

            if (previous_pipes_.find(pipe_name) == previous_pipes_.end())
            {
                // setup dest image that has the same format as the source texture.
                std::string tex_format = "RGBA16";

                // XXX: Assuming we have a depth texture whenever "depth"
                // occurs in the textures name
                if (boost::to_lower_copy(pipe_name).find("depth") != std::string::npos)
                    tex_format = "R32";

                previous_pipes_[pipe_name] = Image::create_2d("Prev-" + pipe_name, 0, 0, tex_format);
                previous_pipes_.at(pipe_name)->clear_image();
            }
            stage->set_shader_input(ShaderInput(std::string("Previous_") + pipe_name, previous_pipes_.at(pipe_name)->get_texture()));
            continue;
        }
        else if (pipe.find("FuturePipe::") == 0)
        {
            // Special case: Future Pipes which are not available yet.
            // They will contain the unmodified data from the last frame.
            const std::regex double_colon_re("::");
            std::vector<std::string> result(std::sregex_token_iterator(pipe.begin(), pipe.end(), double_colon_re, -1), std::sregex_token_iterator());
            std::string pipe_name = result.back();
            self_.debug(fmt::format("Awaiting future pipe {}", pipe_name));
            future_bindings_.push_back({pipe_name, stage});
            continue;
        }

        if (pipes_.find(pipe) == pipes_.end())
        {
            self_.fatal(std::string("Pipe '") + pipe + "' is missing for " + stage->get_debug_name());
            return false;
        }

        stage->set_shader_input(pipes_.at(pipe));
    }

    return true;
}

bool StageManager::Impl::bind_inputs_to_stage(const std::shared_ptr<RenderStage>& stage)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
#else
    VisitorBindToStage vbts;
    vbts.stage = stage;
#endif

    std::vector<std::string> common_inputs = { "MainSceneData", "TimeOfDay" };

    if (!pipeline_.get_setting<bool>("pipeline.stereo_mode"))
    {
        common_inputs.insert(common_inputs.begin(), "mainRender");
        common_inputs.insert(common_inputs.begin(), "mainCam");
    }

    const auto& required_inputs = stage->get_required_inputs();
    common_inputs.insert(common_inputs.begin(), required_inputs.begin(), required_inputs.end());

    for (const auto& input_binding: common_inputs)
    {
        if (inputs_.find(input_binding) == inputs_.end() && input_blocks_.find(input_binding) == input_blocks_.end())
        {
            self_.error(fmt::format("Input {} is missing for {}", input_binding, stage->get_debug_name()));
            continue;
        }

        if (inputs_.find(input_binding) != inputs_.end())
        {
            stage->set_shader_input(inputs_.at(input_binding));
        }
        else if (input_blocks_.find(input_binding) != input_blocks_.end())
        {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
            boost::apply_visitor([&](const auto& block){ block->bind_to(stage); }, input_blocks_.at(input_binding));
#else
            boost::apply_visitor(vbts, input_blocks_.at(input_binding));
#endif
        }
        else
        {
            assert(false && "Input binding not in inputs and not in blocks!");
        }
    }

    return true;
}

void StageManager::Impl::register_stage_result(const std::shared_ptr<RenderStage>& stage)
{
    self_.trace(fmt::format("Registring the result of stage ({}).", stage->get_debug_name()));

    for (const auto& pipe_data: stage->get_produced_pipes())
    {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        if (auto data = boost::get<ShaderInput>(&pipe_data))
            pipes_.insert_or_assign(data->get_name()->get_name(), *data);
        else if (auto data = boost::get<std::shared_ptr<SimpleInputBlock>>(&pipe_data))
            input_blocks_.insert_or_assign((*data)->get_name(), *data);
        else if (auto data = boost::get<std::shared_ptr<GroupedInputBlock>>(&pipe_data))
            input_blocks_.insert_or_assign((*data)->get_name(), *data);
#else
        if (auto data = boost::get<ShaderInput>(&pipe_data))
            pipes_[data->get_name()->get_name()] = *data;
        else if (auto data = boost::get<std::shared_ptr<SimpleInputBlock>>(&pipe_data))
            input_blocks_.insert({(*data)->get_name(), *data});
        else if (auto data = boost::get<std::shared_ptr<GroupedInputBlock>>(&pipe_data))
            input_blocks_.insert({(*data)->get_name(), *data});
#endif
    }

    for (const auto& define: stage->get_produced_defines())
    {
        if (defines_.find(define.first) != defines_.end())
            self_.warn(fmt::format("Stage {} overrides define {}", stage->get_debug_name(), define.first));
        defines_[define.first] = define.second;
    }

    for (const auto& input_data: stage->get_produced_inputs())
    {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        if (auto data = boost::get<ShaderInput>(&input_data))
        {
            const auto& input_name = data->get_name()->get_name();
            if (inputs_.find(input_name) != inputs_.end())
                self_.warn(fmt::format("Stage {} overrides input {}", stage->get_debug_name(), input_name));
            inputs_.insert_or_assign(input_name, *data);
        }
        else if (auto data = boost::get<std::shared_ptr<SimpleInputBlock>>(&input_data))
        {
            input_blocks_.insert_or_assign((*data)->get_name(), *data);
        }
        else if (auto data = boost::get<std::shared_ptr<GroupedInputBlock>>(&input_data))
        {
            input_blocks_.insert_or_assign((*data)->get_name(), *data);
        }
#else
        if (auto data = boost::get<ShaderInput>(&input_data))
        {
            const auto& input_name = data->get_name()->get_name();
            if (inputs_.find(input_name) != inputs_.end())
                self_.warn(fmt::format("Stage {} overrides input {}", stage->get_debug_name(), input_name));
            inputs_[input_name] = *data;
        }
        else if (auto data = boost::get<std::shared_ptr<SimpleInputBlock>>(&input_data))
        {
            input_blocks_.insert({(*data)->get_name(), *data});
        }
        else if (auto data = boost::get<std::shared_ptr<GroupedInputBlock>>(&input_data))
        {
            input_blocks_.insert({(*data)->get_name(), *data});
        }
#endif
    }
}

bool StageManager::Impl::create_previous_pipes()
{
    if (!previous_pipes_.empty())
    {
        prev_stage_ = std::make_shared<UpdatePreviousPipesStage>(pipeline_);
        for (const auto& prev_pipe_tex: previous_pipes_)
        {
            if (pipes_.find(prev_pipe_tex.first) == pipes_.end())
            {
                self_.error(fmt::format("Attempted to use previous frame data from pipe {} - however, that pipe was never created!", prev_pipe_tex.first));
                return false;
            }

            const auto& dest_tex = prev_pipe_tex.second;
            Texture* src_tex = pipes_.at(prev_pipe_tex.first).get_texture();

            // re-setup for layered texture when stereo mode.
            if (src_tex->get_texture_type() == Texture::TextureType::TT_2d_texture_array)
            {
                const std::string& pipe_type = dest_tex->get_texture_format();
                dest_tex->setup_2d_array(0, 0, 1, pipe_type);
                dest_tex->clear_image();
            }

            // Tell the stage to transfer the data from the current pipe to
            // the current texture
            prev_stage_->add_transfer(src_tex, dest_tex->get_texture());
        }

        prev_stage_->create();
        prev_stage_->set_dimensions();
        stages_.push_back(prev_stage_);
    }

    return true;
}

void StageManager::Impl::apply_future_bindings()
{
    for (const auto& pipe_stage: future_bindings_)
    {
        if (pipes_.find(pipe_stage.first) == pipes_.end())
        {
            self_.error(fmt::format("Could not bind future pipe: {} not present!", pipe_stage.first));
            continue;
        }
        pipe_stage.second->set_shader_input(pipes_.at(pipe_stage.first));
    }
    future_bindings_.clear();
}

// ************************************************************************************************

StageManager::StageManager(RenderPipeline& pipeline): RPObject("StageManager"), impl_(std::make_unique<Impl>(*this, pipeline))
{
    impl_->load_stage_order();
}

StageManager::~StageManager() = default;

StageManager::DefinesType& StageManager::get_defines()
{
    return impl_->defines_;
}

const StageManager::DefinesType& StageManager::get_defines() const
{
    return impl_->defines_;
}

void StageManager::add_input(const ShaderInput& inp)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    impl_->inputs_.insert_or_assign(inp.get_name()->get_name(), inp);
#else
    impl_->inputs_[inp.get_name()->get_name()] = inp;
#endif
}

const ShaderInput& StageManager::get_pipe(const std::string& pipe_name) const
{
    if (impl_->pipes_.find(pipe_name) == impl_->pipes_.end())
        return ShaderInput::get_blank();
    else
        return impl_->pipes_.at(pipe_name);
}

void StageManager::add_input_blocks(const std::shared_ptr<SimpleInputBlock>& input_block)
{
    impl_->input_block_list_.push_back(input_block);
}

void StageManager::add_input_blocks(const std::shared_ptr<GroupedInputBlock>& input_block)
{
    impl_->input_block_list_.push_back(input_block);
}

const std::vector<std::shared_ptr<RenderStage>>& StageManager::get_stages() const
{
    return impl_->stages_;
}

void StageManager::add_stage(const std::shared_ptr<RenderStage>& stage)
{
    trace(fmt::format("Adding stage ({}) ...", stage->get_debug_name()));

    bool found = false;
    if (std::find(impl_->stage_order_.begin(), impl_->stage_order_.end(), stage->get_stage_id()) == std::end(impl_->stage_order_))
    {
        error(fmt::format("The stage type {} is not registered yet! Please add it to the StageManager!", stage->get_debug_name()));
        return;
    }

    if (impl_->created_)
    {
        error("Cannot attach stage, stages are already created!");
        return;
    }

    impl_->stages_.push_back(stage);
}

std::shared_ptr<RenderStage> StageManager::get_stage(const std::string& stage_id) const
{
    for (const auto& stage: impl_->stages_)
        if (stage->get_stage_id() == stage_id)
            return stage;
    return std::shared_ptr<RenderStage>(nullptr);
}

void StageManager::setup()
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
#else
    VisitorInsertToInputBlocks vitib(impl_->input_blocks_);
#endif

    debug("Setup Stages ..");
    impl_->created_ = true;

    // Convert input blocks so we can access them in a better way
    for (const auto& block: impl_->input_block_list_)
    {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        boost::apply_visitor([this](const auto& block) {
            std::string block_name = block->get_name();
            impl_->input_blocks_.insert_or_assign(block_name, std::move(block));
        }, block);
#else
        boost::apply_visitor(vitib, block);
#endif
    }
    impl_->input_block_list_.clear();

    impl_->prepare_stages();

    for (auto& stage: impl_->stages_)
    {
        debug(fmt::format("Creating stage ({}) ...", stage->get_debug_name()));
        stage->create();

        trace(fmt::format("Stage ({}) handles window re-sizing.", stage->get_debug_name()));
        stage->handle_window_resize();

        // Rely on the methods to print an appropriate error message
        if (!impl_->bind_pipes_to_stage(stage))
            continue;
        if (!impl_->bind_inputs_to_stage(stage))
            continue;

        impl_->register_stage_result(stage);
    }

    impl_->create_previous_pipes();
    impl_->apply_future_bindings();
}

void StageManager::reload_shaders()
{
    write_autoconfig();
    for (auto& stage: impl_->stages_)
        stage->reload_shaders();
}

void StageManager::update()
{
    for (auto& stage: impl_->stages_)
    {
        if (stage->get_active())
            stage->update();
    }
}

void StageManager::handle_window_resize()
{
    for (auto& stage: impl_->stages_)
        stage->handle_window_resize();
}

void StageManager::write_autoconfig()
{
    debug("Writing shader config");

    // Generate autoconfig as string
    std::string output = "#pragma once\n\n";
    output += "// Autogenerated by the render pipeline\n";
    output += "// Do not edit! Your changes will be lost.\n\n";

    for (const auto& key_val: impl_->defines_)
        output += std::string("#define ") + key_val.first + " " + key_val.second + "\n";

    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
    try
    {
        std::ostream* file = vfs->open_write_file("/$$rptemp/$$pipeline_shader_config.inc.glsl", false, false);
        *file << output;
        vfs->close_write_file(file);
    }
    catch (const std::exception& err)
    {
        error(fmt::format("Error writing shader autoconfig: {}", err.what()));
    }
}

}
