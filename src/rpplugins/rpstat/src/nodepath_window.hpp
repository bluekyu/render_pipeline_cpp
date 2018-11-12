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

#include <nodePath.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>

#include "window_interface.hpp"

namespace rpplugins {

class FileDialog;

class NodePathWindow : public WindowInterface
{
public:
    NodePathWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline);
    virtual ~NodePathWindow();

    void draw_contents() final;

    void set_nodepath(NodePath np);

private:
    void ui_name();
    void ui_write_bam();
    void ui_transform();
    void ui_render_mode();
    void ui_camera_mask();
    void ui_tag();
    void ui_cull_face();
    void ui_depth_test();
    void ui_transparency();
    void ui_flatten(const char* popup_id);
    void ui_effect();

    NodePath np_;
    std::unique_ptr<FileDialog> file_dialog_;

    std::unordered_set<std::string> menu_select_set_;
    std::string name_buffer_;

    int tag_index_ = -1;
};

}
