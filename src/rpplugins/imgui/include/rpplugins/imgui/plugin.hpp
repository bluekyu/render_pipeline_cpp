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
#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

struct ImGuiContext;

class Panda3DImGui;

namespace rpplugins {

class ImGuiPlugin : public rpcore::BasePlugin, public rppanda::DirectObject
{
public:
    static constexpr const char* NEW_FRAME_EVENT_NAME = "imgui-new-frame";
    static constexpr const char* SETUP_CONTEXT_EVENT_NAME = "imgui-setup-context";
    static constexpr const char* DROPFILES_EVENT_NAME = "imgui-dropfiles";

public:
    ImGuiPlugin(rpcore::RenderPipeline& pipeline);
    
    ~ImGuiPlugin() override;

    RequrieType& get_required_plugins(void) const override;

    RENDER_PIPELINE_PLUGIN_DOWNCAST();

    virtual ImGuiContext* get_context() const;
    virtual NodePath get_root() const;

    /** Get dropped files. */
    virtual const std::vector<Filename>& get_dropped_files() const;

    /** Get mouse position when files are dropped. */
    virtual const LVecBase2& get_dropped_point() const;

private:
    void on_load() override;
    void on_window_resized() override;
    void on_unload() override;

    void setup_context(const Event* ev);

    void setup_button();

    static RequrieType require_plugins_;

    std::unique_ptr<Panda3DImGui> p3d_imgui_;
};

}
