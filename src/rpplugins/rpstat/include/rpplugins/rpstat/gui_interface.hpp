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

    virtual void on_imgui_new_frame() = 0;

protected:
    rpcore::RenderPipeline& pipeline_;
    const std::string plugin_id_;
};

// ************************************************************************************************

inline GUIInterface::GUIInterface(rpcore::RenderPipeline& pipeline, const std::string& plugin_id): pipeline_(pipeline), plugin_id_(plugin_id)
{
    auto imgui_plugin = static_cast<ImGuiPlugin*>(pipeline_.get_plugin_mgr()->get_instance("imgui")->downcast());
    ImGui::SetCurrentContext(imgui_plugin->get_context());
    accept(ImGuiPlugin::NEW_FRAME_EVENT_NAME, [this](auto) { on_imgui_new_frame(); });
}

}
