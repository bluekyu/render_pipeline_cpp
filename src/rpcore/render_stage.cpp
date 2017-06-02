#include <render_pipeline/rpcore/render_stage.hpp>

#include <graphicsWindow.h>

#include "render_pipeline/rpcore/loader.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/pluginbase/manager.hpp"
#include "render_pipeline/rpcore/pluginbase/base_plugin.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rpcore {

bool RenderStage::disabled = false;

RenderStage::RenderStage(RenderPipeline& pipeline, const std::string& stage_id): RPObject(stage_id), pipeline_(pipeline), stage_id_(stage_id)
{
}

RenderStage::ProduceType RenderStage::get_produced_inputs(void) const
{
    return {};
}

RenderStage::ProduceType RenderStage::get_produced_pipes(void) const
{
    return {};
}

RenderStage::DefinesType RenderStage::get_produced_defines(void) const
{
    return {};
}

void RenderStage::set_shader_input(const ShaderInput& inp)
{
    for (auto& target: targets_)
        target.second->set_shader_input(inp);
}

void RenderStage::set_active(bool state)
{
    if (active_ != state)
    {
        active_ = state;
        for (auto& target: targets_)
            target.second->set_active(active_);
    }
}

std::shared_ptr<RenderTarget> RenderStage::create_target(const std::string& name)
{
    const std::string& target_name = get_plugin_id() + ":" + stage_id_ + ":" + name;

    if (targets_.find(target_name) != targets_.end())
    {
        error("Overriding existing target: " + target_name);
        return nullptr;
    }

#if _MSC_VER >= 1900
    return targets_.insert_or_assign(target_name, std::make_shared<RenderTarget>(target_name)).first->second;
#else
    return targets_[target_name] = std::make_shared<RenderTarget>(target_name);
#endif
}

void RenderStage::remove_target(const std::shared_ptr<RenderTarget>& target)
{
    target->remove();
    for (auto& key_value: targets_)
    {
        if (target == key_value.second)
        {
            targets_.erase(key_value.first);
            break;
        }
    }
}

PT(Shader) RenderStage::load_shader(const std::vector<std::string>& args, bool stereo_post, bool use_post_gs) const
{
    return get_shader_handle("/$$rp/shader/", args, stereo_post, use_post_gs);
}

PT(Shader) RenderStage::load_plugin_shader(const std::vector<std::string>& args, bool stereo_post, bool use_post_gs) const
{
    const std::string& shader_path = pipeline_.get_plugin_mgr()->get_instance(get_plugin_id())->get_shader_resource("");
    return get_shader_handle(shader_path, args, stereo_post, use_post_gs);
}

void RenderStage::handle_window_resize(void)
{
    set_dimensions();
    for (auto& target: targets_)
        target.second->consider_resize();
}

std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>> RenderStage::prepare_upscaler(int max_invalid_pixels) const
{
    std::shared_ptr<Image> counter = Image::create_counter(get_stage_id() + "-BadPixelsCounter");
    counter->set_clear_color(LColor(0));
    std::shared_ptr<Image> buf = Image::create_buffer(get_stage_id() + "-BadPixels", max_invalid_pixels, "R32I");
    return{ counter, buf };
}

PT(Shader) RenderStage::get_shader_handle(const std::string& base_path, const std::vector<std::string>& args, bool stereo_post, bool use_post_gs) const
{
    static const char* prefix[] ={"/$$rpconfig", "/$$rp/shader", "/$$rptemp"};

    if (args.size() <= 0 || args.size() > 3)
        throw std::runtime_error("args.size() <= 0 || args.size() > 3");

    std::vector<std::string> path_args;

    for (const auto& source: args)
    {
        bool found = false;
        for (size_t k = 0; k < std::extent<decltype(prefix)>::value; k++)
        {
            if (source.find(prefix[k]) != std::string::npos)
            {
                found = true;
                path_args.push_back(source);
                break;
            }
        }

        if (!found)
            path_args.push_back(base_path + source);
    }

    if (args.size() == 1)
    {
        if (stereo_post)
        {
            path_args.insert(path_args.begin(), "/$$rp/shader/default_post_process_stereo.vert.glsl");

            // NVIDIA single stereo rendering extension OR
            // force layered rendering.
            if (pipeline_.get_stage_mgr()->get_defines().at("NVIDIA_STEREO_VIEW") == "0" || use_post_gs)
                path_args.insert(path_args.end(), "/$$rp/shader/default_post_process_stereo.geom.glsl");
        }
        else
        {
            path_args.insert(path_args.begin(), "/$$rp/shader/default_post_process.vert.glsl");
        }
    }

    return RPLoader::load_shader(path_args);
}

}
