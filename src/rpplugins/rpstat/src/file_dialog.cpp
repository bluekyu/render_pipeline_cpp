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

#include "file_dialog.hpp"

#include <imgui.h>

#include "imgui/imgui_stdlib.h"
#include "rpplugins/rpstat/plugin.hpp"

namespace rpplugins {

static constexpr const char* WARNING_POPUP_ID = "Warning###FileDialogWarning";
static constexpr const char* ERROR_POPUP_ID = "Error###FileDialogError";

FileDialog::FileDialog(RPStatPlugin& plugin, OperationFlag op_flag, const std::string& id):
    Dialog(id), plugin_(plugin), operation_flag_(op_flag)
{
}

void FileDialog::draw_contents()
{
    draw_file_input();

    draw_buttons();

    draw_warning_popup();
    draw_error_popup();
}

void FileDialog::accept()
{
    Filename fname(buffer_);
    if (operation_flag_ == OperationFlag::open)
    {
        if (fname.exists())
        {
            fname_ = Filename(buffer_);
            Dialog::accept();
        }
        else
        {
            open_error_popup("The file does not exist!");
        }
    }
    else if (operation_flag_ == OperationFlag::write)
    {
        if (fname.exists())
        {
            open_warning_popup("The file already exists! Do you want to overwrite it?");
        }
        else if (!Filename(fname.get_dirname()).exists())
        {
            open_error_popup("Parent directory does not exist!");
        }
        else
        {
            fname_ = Filename(buffer_);
            Dialog::accept();
        }
    }
}

void FileDialog::open_warning_popup(const std::string& msg)
{
    popup_message_ = msg;
    ImGui::OpenPopup(WARNING_POPUP_ID);
}

void FileDialog::open_error_popup(const std::string& msg)
{
    popup_message_ = msg;
    ImGui::OpenPopup(ERROR_POPUP_ID);
}

void FileDialog::process_file_drop()
{
    if (ImGui::IsItemHovered() && plugin_.is_file_dropped())
    {
        const auto& files = plugin_.get_dropped_files();
        if (files.size() > 0)
        {
            auto f = files[0];
            if (!f.empty())
                buffer_ = f;
        }

        plugin_.unset_file_dropped();
    }
}

void FileDialog::draw_file_input()
{
    ImGui::InputText("###FileDialogPath", &buffer_);

    process_file_drop();

    ImGui::SameLine();

    ImGui::Button("Browse");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Currently, this is not implemented.\nUse drag & drop the file.");
}

void FileDialog::draw_warning_popup()
{
    if (!ImGui::BeginPopupModal(WARNING_POPUP_ID))
        return;

    ImGui::Text(popup_message_.c_str());
    if (ImGui::Button("OK"))
    {
        fname_ = Filename(buffer_);
        Dialog::accept();
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Close"))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

void FileDialog::draw_error_popup()
{
    if (!ImGui::BeginPopupModal(ERROR_POPUP_ID))
        return;

    ImGui::Text(popup_message_.c_str());
    if (ImGui::Button("Close"))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

}
