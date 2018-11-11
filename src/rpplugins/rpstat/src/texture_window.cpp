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

#include "texture_window.hpp"

#include <fmt/ostream.h>

#include <paramNodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>

#include "rpplugins/rpstat/plugin.hpp"
#include "scenegraph_window.hpp"

namespace rpplugins {

TextureWindow::TextureWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline) : WindowInterface(plugin, pipeline, "Texture Window", "###Texture")
{
    accept(
        ScenegraphWindow::NODE_SELECTED_EVENT_NAME,
        [this](const Event* ev) { set_nodepath(DCAST(ParamNodePath, ev->get_parameter(0).get_ptr())->get_value()); }
    );

    for (int k = 0; k <= int(Texture::TextureType::TT_1d_texture_array); ++k)
        texture_type_cache_list_.push_back(Texture::format_texture_type(Texture::TextureType(k)));

    for (int k = 0; k <= int(Texture::ComponentType::T_unsigned_int); ++k)
        component_type_cache_list_.push_back(Texture::format_component_type(Texture::ComponentType(k)));

    for (int k = int(Texture::Format::F_depth_stencil); k <= int(Texture::Format::F_r16i); ++k)
        texture_format_cache_list_.push_back(Texture::format_format(Texture::Format(k)));
}

void TextureWindow::draw_contents()
{
    if (!np_)
        return;

    if (tex_collection_.get_num_textures() == 0)
        return;

    static const char* empty_name = "(no-name)";

    texture_names_.clear();
    for (int k = 0, k_end = tex_collection_.get_num_textures(); k < k_end; ++k)
    {
        auto tex = tex_collection_.get_texture(k);
        if (tex->has_name())
            texture_names_.push_back(tex_collection_.get_texture(k)->get_name().c_str());
        else
            texture_names_.push_back(empty_name);
    }

    ImGui::Combo("Textures", &current_item_, texture_names_.data(), texture_names_.size());

    Texture* tex = tex_collection_.get_texture(current_item_);

    ImGui::Text("Preview");
    ImGui::Image(
        tex,
        ImVec2(256, 256),
        ImVec2(0, 0),
        ImVec2(1, 1),
        ImColor(255, 255, 255, 255),
        ImColor(255, 255, 255, 128));

    ui_texture_type(tex);
    ui_component_type(tex);
    ui_texture_format(tex);

    ImGui::LabelText("Size", "%d x %d x %d", tex->get_x_size(), tex->get_y_size(), tex->get_z_size());
}

void TextureWindow::set_nodepath(NodePath np)
{
    np_ = np;
    current_item_ = 0;

    if (is_open_)
        tex_collection_ = np_.find_all_textures();
}

void TextureWindow::show()
{
    tex_collection_ = np_.find_all_textures();
    WindowInterface::show();
}

void TextureWindow::ui_texture_type(Texture* tex)
{
    ImGui::LabelText("Texture Type", Texture::format_texture_type(tex->get_texture_type()).c_str());
}

void TextureWindow::ui_component_type(Texture* tex)
{
    static auto getter = [](void* data, int idx, const char** out_text) {
        const auto& cache_list = *reinterpret_cast<decltype(component_type_cache_list_)*>(data);
        *out_text = cache_list[idx].c_str();
        return true;
    };

    int item_current = static_cast<int>(tex->get_component_type());
    if (ImGui::Combo("Component Type", &item_current, getter, &component_type_cache_list_, static_cast<int>(component_type_cache_list_.size())))
    {
        tex->set_component_type(static_cast<Texture::ComponentType>(item_current));
    }
}

void TextureWindow::ui_texture_format(Texture* tex)
{
    static const int start_enum_value = int(Texture::Format::F_depth_stencil);

    static auto getter = [](void* data, int idx, const char** out_text) {
        const auto& cache_list = *reinterpret_cast<decltype(texture_format_cache_list_)*>(data);
        *out_text = cache_list[idx].c_str();
        return true;
    };

    int item_current = static_cast<int>(tex->get_format()) - start_enum_value;
    if (ImGui::Combo("Format", &item_current, getter, &texture_format_cache_list_, static_cast<int>(texture_format_cache_list_.size())))
    {
        tex->set_format(static_cast<Texture::Format>(item_current + start_enum_value));
    }
}

}
