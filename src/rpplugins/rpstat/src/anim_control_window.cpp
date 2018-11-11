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

#include "anim_control_window.hpp"

#include <animControl.h>

#include <fmt/ostream.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>

#include "rpplugins/rpstat/plugin.hpp"

namespace rpplugins {

size_t AnimControlWindow::global_id_ = 0;
std::unordered_map<AnimControl*, AnimControlWindow*> AnimControlWindow::holder_;

std::pair<AnimControlWindow*, bool> AnimControlWindow::create_window(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, AnimControl* control)
{
    auto found = holder_.find(control);
    if (found != holder_.end())
    {
        return { found->second, false };
    }
    else
    {
        auto window = std::unique_ptr<AnimControlWindow>(new AnimControlWindow(plugin, pipeline, control));
        auto window_raw = window.get();
        plugin.add_window(std::move(window));
        return { holder_.emplace(control, window_raw).first->second, true };
    }
}

void AnimControlWindow::remove_window(AnimControlWindow* window)
{
    holder_.erase(window->control_);
    window->plugin_.remove_window(window);
}

AnimControlWindow::AnimControlWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, AnimControl* control) :
    WindowInterface(plugin, pipeline, "Anim Control Window", "###AnimControl" + std::to_string(global_id_++)), control_(control)
{
    reset();
}

void AnimControlWindow::draw()
{
    is_open_ = true;

    title_ = fmt::format("Anim Control Window: {}", control_->get_name());
    if (!ImGui::Begin(fmt::format("{}{}", title_, unique_id_).c_str(), &is_open_, window_flags_))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    draw_contents();

    ImGui::End();

    // auto destruct when closed
    if (!is_open_)
        remove_window(this);
}

void AnimControlWindow::draw_contents()
{
    float frame = static_cast<float>(control_->get_full_fframe());
    if (ImGui::DragFloat("Frame", &frame, 0.125f))
        control_->pose(frame);

    ImGui::InputFloat("From", &from_frame_);
    ImGui::InputFloat("To", &to_frame_);

    float play_rate = static_cast<float>(control_->get_play_rate());
    if (ImGui::InputFloat("Play Rate", &play_rate))
        control_->set_play_rate(play_rate);

    ImGui::Checkbox("Restart", &restart_);

    if (ImGui::Button("Play"))
        control_->play(from_frame_, to_frame_);

    ImGui::SameLine();

    if (ImGui::Button("Loop"))
        control_->loop(restart_, from_frame_, to_frame_);

    ImGui::SameLine();

    if (ImGui::Button("Pingpong"))
        control_->pingpong(restart_, from_frame_, to_frame_);

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
        control_->stop();

    if (ImGui::Button("Reset"))
    {
        control_->pose(0);
        control_->set_play_rate(1);
        reset();
    }
}

void AnimControlWindow::reset()
{
    from_frame_ = 0;
    to_frame_ = static_cast<float>(control_->get_num_frames() - 1);
    restart_ = false;
}

}
