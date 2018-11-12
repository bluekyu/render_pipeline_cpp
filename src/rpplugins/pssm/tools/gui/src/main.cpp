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

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/pluginbase/setting_types.hpp>

#include <rpplugins/rpstat/gui_interface.hpp>

#include <pssm_plugin.hpp>

namespace rpplugins {

class PluginGUI : public GUIInterface
{
public:
    PluginGUI(rpcore::RenderPipeline& pipeline);
    virtual ~PluginGUI() = default;

    void on_draw_menu() override;
    void on_draw_new_frame() override;

private:
    rpcore::PluginManager* plugin_mgr_;
    PSSMPlugin* plugin_;
    bool is_open_ = false;

    rpcore::FloatType* max_distance_;
};

// ************************************************************************************************

PluginGUI::PluginGUI(rpcore::RenderPipeline& pipeline): GUIInterface(pipeline, RPPLUGINS_GUI_ID_STRING)
{
    plugin_mgr_ = pipeline_.get_plugin_mgr();
    plugin_ = static_cast<decltype(plugin_)>(plugin_mgr_->get_instance(RPPLUGINS_GUI_ID_STRING)->downcast());

    max_distance_ = static_cast<rpcore::FloatType*>(plugin_mgr_->get_setting_handle(RPPLUGINS_GUI_ID_STRING, "max_distance")->downcast());
}

void PluginGUI::on_draw_menu()
{
    if (ImGui::MenuItem("PSSM"))
        is_open_ = true;
}

void PluginGUI::on_draw_new_frame()
{
    if (!is_open_)
        return;

    if (!ImGui::Begin("PSSM Plugin", &is_open_))
        return ImGui::End();

    float max_distance = boost::any_cast<float>(max_distance_->get_value());
    if (ImGui::InputFloat(max_distance_->get_label().c_str(), &max_distance, 0, 0, 3, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        max_distance_->set_value(max_distance);
        plugin_mgr_->on_setting_changed(RPPLUGINS_GUI_ID_STRING, "max_distance");
    }

    ImGui::End();
}

}

RPPLUGINS_GUI_CREATOR(rpplugins::PluginGUI)
