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

#include "day_manager_window.hpp"

#include <regex>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>

namespace rpplugins {

DayManagerWindow::DayManagerWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline) : WindowInterface(plugin, pipeline, "Day Manager", "###Day Manager")
{
    manager_ = pipeline_.get_daytime_mgr();

    formatted_time_[std::extent<decltype(formatted_time_)>::value - 1] = '\0';
}

void DayManagerWindow::draw_contents()
{
    static bool invalid_ft = false;

    if (invalid_ft)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.7f, 0.0f, 0.0f, 0.5f));

    const bool ft_changed = ImGui::InputText("Formatted Time", formatted_time_, std::extent<decltype(formatted_time_)>::value);

    if (invalid_ft)
        ImGui::PopStyleColor();

    if (ft_changed)
    {
        std::smatch match;
        std::string ft(formatted_time_);
        if (std::regex_match(ft, match, std::regex("^[0-9]{1,2}:[0-9]{1,2}$")))
        {
            invalid_ft = false;
            manager_->set_time(ft);
        }
        else
        {
            invalid_ft = true;
        }
    }

    float t = manager_->get_time();
    if (ImGui::SliderFloat("Time", &t, 0.0f, 1.0f))
    {
        manager_->set_time(t);

        // sync GUI
        std::memcpy(formatted_time_, manager_->get_formatted_time().c_str(), std::extent<decltype(formatted_time_)>::value - 1);
    }
}

void DayManagerWindow::show()
{
    std::memcpy(formatted_time_, manager_->get_formatted_time().c_str(), std::extent<decltype(formatted_time_)>::value-1);
    WindowInterface::show();
}

}
