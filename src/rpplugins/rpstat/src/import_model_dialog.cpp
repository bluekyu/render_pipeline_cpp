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

#include "import_model_dialog.hpp"

#include <imgui.h>

#include "imgui/imgui_stl.h"
#include "rpplugins/rpstat/plugin.hpp"

namespace rpplugins {

void ImportModelDialog::draw_contents()
{
    draw_file_input();

    if (ImGui::Button("OK"))
    {
        Filename fname(buffer_);
        if (operation_flag_ == OperationFlag::open)
        {
            if (fname.exists())
                accept();
            else
                open_error_popup("The file does not exist!");
        }
        else if (operation_flag_ == OperationFlag::write)
        {
            if (fname.exists())
                open_warning_popup("The file already exists! Do you want to overwrite it?");
            else if (!Filename(fname.get_dirname()).exists())
                open_error_popup("Parent directory does not exist!");
            else
                accept();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Close"))
        reject();

    draw_warning_popup();
    draw_error_popup();
}

}
