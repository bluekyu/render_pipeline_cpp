/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <render_pipeline/rppanda/showbase/direct_object.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>

#include <imgui.h>

#include <rpplugins/imgui/plugin.hpp>

namespace rpplugins {

#define RPPLUGINS_GUI_CREATOR(PLUGIN_TYPE) \
    static std::unique_ptr<::rpplugins::GUIInterface> rpplugins_gui_creator__(::rpcore::RenderPipeline& pipeline) \
    { \
        return std::make_unique<PLUGIN_TYPE>(pipeline); \
    } \
    BOOST_DLL_ALIAS(::rpplugins_gui_creator__, create_gui)

class GUIInterface : public rppanda::DirectObject
{
public:
    GUIInterface(rpcore::RenderPipeline& pipeline, const std::string& plugin_id);
    virtual ~GUIInterface() = default;

    virtual void on_draw_menu() {}
    virtual void on_draw_new_frame() = 0;

    rpcore::PluginManager* get_plugin_mgr() const;
    const std::string& get_plugin_id() const;

    template <class T>
    T* get_setting_handle(const std::string& setting_id) const;

protected:
    rpcore::RenderPipeline& pipeline_;
    rpcore::PluginManager* plugin_mgr_;
    const std::string plugin_id_;
};

// ************************************************************************************************

inline GUIInterface::GUIInterface(rpcore::RenderPipeline& pipeline, const std::string& plugin_id): pipeline_(pipeline), plugin_id_(plugin_id)
{
    plugin_mgr_ = pipeline_.get_plugin_mgr();
    ImGui::SetCurrentContext(static_cast<ImGuiPlugin*>(plugin_mgr_->get_instance("imgui")->downcast())->get_context());
}

inline rpcore::PluginManager* GUIInterface::get_plugin_mgr() const
{
    return plugin_mgr_;
}

inline const std::string& GUIInterface::get_plugin_id() const
{
    return plugin_id_;
}

template <class T>
T* GUIInterface::get_setting_handle(const std::string& setting_id) const
{
    return static_cast<T*>(plugin_mgr_->get_setting_handle(plugin_id_, setting_id)->downcast());
}

}
