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

#include "dialog.hpp"

#include <imgui.h>

namespace rpplugins {

void Dialog::open()
{
    will_open_ = true;
    accepted_.reset();
}

const boost::optional<bool>& Dialog::draw()
{
    if (will_open_)
    {
        ImGui::OpenPopup(id_.c_str());
        will_open_ = false;
    }

    if (!ImGui::BeginPopupModal(id_.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static const boost::optional<bool> not_opened;
        return not_opened;
    }

    draw_contents();

    if (accepted_)
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();

    return accepted_;
}

void Dialog::draw_contents()
{
    draw_buttons();
}

void Dialog::draw_buttons()
{
    if (ImGui::Button("OK"))
        accept();

    ImGui::SameLine();

    if (ImGui::Button("Close"))
        reject();
}

void Dialog::accept()
{
    accepted_ = true;
}

void Dialog::reject()
{
    accepted_ = false;
}

// ************************************************************************************************

void MessageDialog::draw_contents()
{
    ImGui::TextUnformatted(message_.c_str());
    draw_buttons();
}

}
