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

namespace rpplugins {

class ImGuiPlugin;

class GUIInterface;

class WindowInterface;
class ScenegraphWindow;

class RPStatPlugin : public rpcore::BasePlugin, public rppanda::DirectObject
{
public:
    RPStatPlugin(rpcore::RenderPipeline& pipeline);
    
    virtual ~RPStatPlugin();

    RequrieType& get_required_plugins(void) const override;

    RENDER_PIPELINE_PLUGIN_DOWNCAST();

    void on_load() override;
    void on_pipeline_created() override;
    void on_unload() override;

    NodePath get_copied_nodepath() const;
    void set_copied_nodepath(NodePath np);

    bool is_file_dropped() const;
    void unset_file_dropped();
    const std::vector<Filename>& get_dropped_files() const;

    ScenegraphWindow* get_scenegraph_window() const;

    void add_window(std::unique_ptr<WindowInterface> window);
    std::unique_ptr<WindowInterface> remove_window(WindowInterface* window);

private:
    typedef std::unique_ptr<GUIInterface> (GUICreatorType)(rpcore::RenderPipeline&);

    void load_plugin_gui(const std::string& plugin_id);

    void on_imgui_new_frame();

    void draw_main_menu_bar();

    static RequrieType require_plugins_;

    ImGuiPlugin* imgui_plugin_;
    std::unordered_map<std::string, std::pair<std::function<GUICreatorType>, std::unique_ptr<GUIInterface>>> gui_instances_;

    std::list<std::unique_ptr<WindowInterface>> windows_;
    ScenegraphWindow* scenegraph_window_;

    NodePath copied_np_;

    bool file_dropped_ = false;
};

// ************************************************************************************************

inline NodePath RPStatPlugin::get_copied_nodepath() const
{
    return copied_np_;
}

inline void RPStatPlugin::set_copied_nodepath(NodePath np)
{
    copied_np_ = np;
}

inline bool RPStatPlugin::is_file_dropped() const
{
    return file_dropped_;
}

inline void RPStatPlugin::unset_file_dropped()
{
    file_dropped_ = false;
}

inline ScenegraphWindow* RPStatPlugin::get_scenegraph_window() const
{
    return scenegraph_window_;
}

}
