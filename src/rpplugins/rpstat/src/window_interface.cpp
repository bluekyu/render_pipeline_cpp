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

#include "window_interface.hpp"

#include <algorithm>

#include <fmt/ostream.h>

#include <render_pipeline/rppanda/showbase/messenger.hpp>

#include "rpplugins/rpstat/plugin.hpp"

namespace rpplugins {

size_t WindowInterface::window_count_ = 0;

WindowInterface::~WindowInterface() = default;

void WindowInterface::send_show_event(const std::string& unique_id)
{
    rppanda::Messenger::get_global_instance()->send(SHOW_WINDOW_EVENT_NAME_PREFIX + unique_id);
}

WindowInterface::WindowInterface(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, const std::string& title = "no-name") :
    WindowInterface(plugin, pipeline, title, "##" + std::to_string(window_count_))
{
}

WindowInterface::WindowInterface(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, const std::string& title, const std::string& unique_id):
    plugin_(plugin), pipeline_(pipeline), title_(title), window_id_(window_count_++), unique_id_(unique_id)
{
    accept(SHOW_WINDOW_EVENT_NAME_PREFIX + unique_id_, [this](const Event*) { show(); });
}

void WindowInterface::draw()
{
    if (!is_open_)
        return;

    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(fmt::format("{}{}", title_, unique_id_).c_str(), &is_open_, window_flags_))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    draw_contents();

    ImGui::End();
}

}
