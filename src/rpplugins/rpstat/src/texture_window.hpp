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

#include "window_interface.hpp"

namespace rpplugins {

class TextureWindow : public WindowInterface
{
public:
    static constexpr const char* TEXTURE_SELECTED_EVENT_NAME = "rpstat-texture-selected";

public:
    TextureWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline);
    virtual ~TextureWindow();

    void draw_contents() final;

    void set_nodepath(NodePath np);

    void show() final;

private:
    void ui_texture_type(Texture* tex);
    static bool texture_type_list_cache_getter(void* data, int idx, const char** out_text);

    void ui_component_type(Texture* tex);
    void ui_texture_format(Texture* tex);

    NodePath np_;
    TextureCollection tex_collection_;
    int current_item_ = 0;
    std::vector<const char*> texture_names_;

    std::vector<std::string> texture_type_cache_list_;
    std::vector<std::string> component_type_cache_list_;
    std::vector<std::string> texture_format_cache_list_;
};

}
