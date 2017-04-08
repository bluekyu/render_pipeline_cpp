#include "render_pipeline/rpcore/pluginbase/day_manager.h"

#include <boost/format.hpp>

#include <regex>
#include <virtualFileSystem.h>

#include "render_pipeline/rpcore/render_pipeline.h"
#include "render_pipeline/rpcore/stage_manager.h"
#include "render_pipeline/rpcore/pluginbase/manager.h"
#include "render_pipeline/rpcore/pluginbase/day_setting_types.h"
#include "render_pipeline/rpcore/util/shader_input_blocks.h"
#include "render_pipeline/rplibs/py_to_cpp.hpp"

namespace rpcore {

struct DayTimeManager::Impl
{
    Impl(RenderPipeline& pipeline);

    RenderPipeline& pipeline_;

    std::shared_ptr<GroupedInputBlock> input_ubo_;
    std::unordered_map<std::string, std::shared_ptr<DayBaseType>> setting_handles_;
    float time_ = 0.5f;
};

DayTimeManager::Impl::Impl(RenderPipeline& pipeline): pipeline_(pipeline)
{
    input_ubo_ = std::make_shared<GroupedInputBlock>("TimeOfDay");
}

// ************************************************************************************************

DayTimeManager::DayTimeManager(RenderPipeline& pipeline): RPObject("DayTimeManager"), impl_(std::make_unique<Impl>(pipeline))
{
}

DayTimeManager::~DayTimeManager(void) = default;

float DayTimeManager::get_time(void) const
{
    return impl_->time_;
}

void DayTimeManager::set_time(float time)
{
    impl_->time_ = rplibs::py_fmod(time, 1.0f);
}

void DayTimeManager::set_time(const std::string& time)
{
    const std::regex colon_re(":");
    std::vector<std::string> parsed_val(std::sregex_token_iterator(time.begin(), time.end(), colon_re, -1), std::sregex_token_iterator());

    if (parsed_val.size() != 2)
    {
        warn(std::string("Invalid time format: ") + time);
        return;
    }
    impl_->time_ = (std::stoi(parsed_val[0]) * 60.0f + std::stoi(parsed_val[1])) / (24.0f * 60.0f);
}

std::string DayTimeManager::get_formatted_time(void) const
{
    static boost::format time_format("%02d:%02d");

    float total_minutes = impl_->time_ * 24 * 60;
    return (time_format % (int(total_minutes) / 60) % (int(total_minutes) % 60)).str();
}

void DayTimeManager::load_settings(void)
{
    const auto& day_settings = impl_->pipeline_.get_plugin_mgr()->get_day_settings();
    for (const auto& id_settings: day_settings)
    {
        const std::string plugin_id(id_settings.first);

        for (const auto& setting_handle: id_settings.second)
        {
            const std::string setting_id = plugin_id + "." + setting_handle.first;
            impl_->input_ubo_->register_pta(setting_id, setting_handle.second->get_glsl_type());
            impl_->setting_handles_[setting_id] = setting_handle.second;
        }
    }
    impl_->pipeline_.get_stage_mgr()->add_input_blocks(impl_->input_ubo_);

    // Generate UBO shader code
    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
    try
    {
        // Try to write the temporary file
        std::ostream* file = vfs->open_write_file("/$$rptemp/$$daytime_config.inc.glsl", false, false);
        *file << (impl_->input_ubo_->generate_shader_code());
        vfs->close_write_file(file);
    }
    catch (const std::exception& err)
    {
        error(std::string("Failed to write DayTimeManager UBO shader code! ") + err.what());
    }
}

void DayTimeManager::update(void)
{
    for (const auto& id_handle: impl_->setting_handles_)
    {
        // XXX: Find a better interface for this.Without this fix, colors
        // are in the range 0 .. 255 in the shader.
        const auto& color_handle = std::dynamic_pointer_cast<ColorType>(id_handle.second);
        DayBaseType::ValueType value;
        if (color_handle)
            value = color_handle->get_value_at(impl_->time_);
        else
            value = std::dynamic_pointer_cast<ScalarType>(id_handle.second)->get_scaled_value_at(impl_->time_);

        if (value.second == 1)
            impl_->input_ubo_->update_input(id_handle.first, value.first[0]);
        else
            impl_->input_ubo_->update_input(id_handle.first, value.first);
    }
}

}
