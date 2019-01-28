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

#include <rpplugins/rpstat/gui_interface.hpp>
#include <rpplugins/rpstat/gui_helper.hpp>

namespace rpplugins {

class PluginGUI : public GUIInterface
{
public:
    PluginGUI(rpcore::RenderPipeline& pipeline);
    virtual ~PluginGUI() = default;

    void on_draw_menu() override;
    void on_draw_new_frame() override;

    void reset();

private:
    bool is_open_ = false;

    float max_distance_ui_;
    rpcore::FloatType* max_distance_;

    float logarithmic_factor_ui_;
    rpcore::FloatType* logarithmic_factor_;

    float sun_distance_ui_;
    rpcore::FloatType* sun_distance_;
};

// ************************************************************************************************

PluginGUI::PluginGUI(rpcore::RenderPipeline& pipeline): GUIInterface(pipeline, RPPLUGINS_GUI_ID_STRING)
{
    max_distance_ = static_cast<rpcore::FloatType*>(get_setting_handle("max_distance")->downcast());
    logarithmic_factor_ = static_cast<rpcore::FloatType*>(get_setting_handle("logarithmic_factor")->downcast());
    sun_distance_ = static_cast<rpcore::FloatType*>(get_setting_handle("sun_distance")->downcast());
}

void PluginGUI::on_draw_menu()
{
    if (!ImGui::MenuItem("PSSM"))
        return;

    is_open_ = true;
    reset();
}

void PluginGUI::on_draw_new_frame()
{
    if (!is_open_)
        return;

    if (!ImGui::Begin("PSSM Plugin", &is_open_))
        return ImGui::End();

    draw_slider(this, max_distance_, max_distance_ui_);
    draw_slider(this, logarithmic_factor_, logarithmic_factor_ui_);
    draw_slider(this, sun_distance_, sun_distance_ui_);

    if (ImGui::Button("Apply"))
    {
        std::unordered_set<std::string> changed_settings;

        check_setting_changed(changed_settings, "max_distance", max_distance_, max_distance_ui_);
        check_setting_changed(changed_settings, "logarithmic_factor", logarithmic_factor_, logarithmic_factor_ui_);
        check_setting_changed(changed_settings, "sun_distance", sun_distance_, sun_distance_ui_);

        plugin_mgr_->on_setting_changed({ { plugin_id_, changed_settings } });
    }

    ImGui::End();
}

void PluginGUI::reset()
{
    max_distance_ui_ = max_distance_->get_value_as_type();
    logarithmic_factor_ui_ = logarithmic_factor_->get_value_as_type();
    sun_distance_ui_ = sun_distance_->get_value_as_type();
}

}

RPPLUGINS_GUI_CREATOR(rpplugins::PluginGUI)
