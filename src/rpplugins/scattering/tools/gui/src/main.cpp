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

    float ground_reflectance_ui_;
    rpcore::FloatType* ground_reflectance_;

    float rayleigh_factor_ui_;
    rpcore::FloatType* rayleigh_factor_;

    float rayleigh_height_scale_ui_;
    rpcore::FloatType* rayleigh_height_scale_;
};

// ************************************************************************************************

PluginGUI::PluginGUI(rpcore::RenderPipeline& pipeline): GUIInterface(pipeline, RPPLUGINS_GUI_ID_STRING)
{
    ground_reflectance_ = get_setting_handle<rpcore::FloatType>("ground_reflectance");
    rayleigh_factor_ = get_setting_handle<rpcore::FloatType>("rayleigh_factor");
    rayleigh_height_scale_ = get_setting_handle<rpcore::FloatType>("rayleigh_height_scale");
}

void PluginGUI::on_draw_menu()
{
    if (!ImGui::MenuItem("Scattering"))
        return;

    is_open_ = true;
    reset();
}

void PluginGUI::on_draw_new_frame()
{
    if (!is_open_)
        return;

    if (!ImGui::Begin("Scattering Plugin", &is_open_))
        return ImGui::End();

    draw_slider(this, ground_reflectance_, ground_reflectance_ui_);
    draw_slider(this, rayleigh_factor_, rayleigh_factor_ui_);
    draw_slider(this, rayleigh_height_scale_, rayleigh_height_scale_ui_);

    if (ImGui::Button("Apply"))
    {
        std::unordered_set<std::string> changed_settings;

        check_setting_changed(changed_settings, "ground_reflectance", ground_reflectance_, ground_reflectance_ui_);
        check_setting_changed(changed_settings, "rayleigh_factor", rayleigh_factor_, rayleigh_factor_ui_);
        check_setting_changed(changed_settings, "rayleigh_height_scale", rayleigh_height_scale_, rayleigh_height_scale_ui_);

        plugin_mgr_->on_setting_changed({ { plugin_id_, changed_settings } });
    }

    ImGui::End();
}

void PluginGUI::reset()
{
    ground_reflectance_ui_ = ground_reflectance_->get_value();
    rayleigh_factor_ui_ = rayleigh_factor_->get_value();
    rayleigh_height_scale_ui_ = rayleigh_height_scale_->get_value();
}

}

RPPLUGINS_GUI_CREATOR(rpplugins::PluginGUI)
