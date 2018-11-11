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

#include "material_window.hpp"

#include <fmt/ostream.h>

#include <paramNodePath.h>

#include <render_pipeline/rpcore/util/rpmaterial.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>

#include "rpplugins/rpstat/plugin.hpp"

#include "imgui/imgui_stl.h"
#include "scenegraph_window.hpp"

namespace rpplugins {

MaterialWindow::MaterialWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline) : WindowInterface(plugin, pipeline, "Material Window", "###Material")
{
    window_flags_ |= ImGuiWindowFlags_MenuBar;

    accept(
        ScenegraphWindow::NODE_SELECTED_EVENT_NAME,
        [this](const Event* ev) { set_nodepath(DCAST(ParamNodePath, ev->get_parameter(0).get_ptr())->get_value()); }
    );

    accept(
        MATERIAL_SELECTED_EVENT_NAME,
        [this](const Event* ev) { set_material(DCAST(Material, ev->get_parameter(0).get_ptr())); }
    );
}

void MaterialWindow::set_nodepath(NodePath np)
{
    np_ = np;
    current_item_ = -1;

    if (is_open_)
        mat_collection_ = np.find_all_materials();
}

void MaterialWindow::set_material(Material* mat)
{
    np_.clear();
    current_item_ = -1;
    mat_collection_.clear();
    mat_collection_.add_material(mat);
}

void MaterialWindow::show()
{
    if (np_)
        mat_collection_ = np_.find_all_materials();
    WindowInterface::show();
}

void MaterialWindow::draw_contents()
{
    static bool show_panda_material = false;

    const bool not_empty = !np_.is_empty();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Show Panda Material", nullptr, &show_panda_material);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (mat_collection_.get_num_materials() == 0)
        return;

    static std::string name_buffer;
    if (current_item_ == -1)
    {
        current_item_ = 0;
        name_buffer = mat_collection_.get_material(current_item_)->get_name();
    }

    {
        static auto getter = [](void* data, int idx, const char** out_text) {
            static const char* empty_name = "(no-name)";

            MaterialCollection* mat_collection = static_cast<MaterialCollection*>(data);

            auto mat = mat_collection->get_material(idx);
            if (mat->has_name())
                *out_text = mat->get_name().c_str();
            else
                *out_text = empty_name;

            return true;
        };

        if (ImGui::Combo("Materials", &current_item_, getter, &mat_collection_, mat_collection_.size()))
            name_buffer = mat_collection_.get_material(current_item_)->get_name();
    }

    if (ImGui::InputText("Name", &name_buffer, ImGuiInputTextFlags_EnterReturnsTrue))
        mat_collection_.get_material(current_item_)->set_name(name_buffer);

    if (show_panda_material)
        draw_panda_material();
    else
        draw_rp_material();
}

void MaterialWindow::draw_rp_material()
{
    rpcore::RPMaterial mat(mat_collection_.get_material(current_item_));

    static const char* shading_models[] = { "Default", "Emissive", "Clearcoat", "Transparent", "Skin", "Foliage", "Unlit" };
    static_assert(static_cast<int>(rpcore::RPMaterial::ShadingModel::UNLIT_MODEL) == std::extent<decltype(shading_models)>::value-1, "API is changed! Update this code.");

    int shading_model = static_cast<int>(mat.get_shading_model());
    if (ImGui::ListBox("Render Mode\n(single select)", &shading_model, shading_models, std::extent<decltype(shading_models)>::value, 5))
        mat.set_shading_model(static_cast<rpcore::RPMaterial::ShadingModel>(shading_model));

    auto base_color = mat.get_base_color();
    if (ImGui::ColorEdit4("Base Color", &base_color[0]))
        mat.set_base_color(base_color);

    float normal_factor = mat.get_normal_factor();
    if (ImGui::SliderFloat("Normal Factor", &normal_factor, 0, 1))
        mat.set_normal_factor(normal_factor);

    float roughness = mat.get_roughness();
    if (ImGui::SliderFloat("Roughness", &roughness, 0, 1))
        mat.set_roughness(roughness);

    float specular_ior = mat.get_specular_ior();
    if (ImGui::SliderFloat("Specular IOR", &specular_ior, 1, 2.51f))
        mat.set_specular_ior(specular_ior);

    float metallic = mat.get_metallic();
    if (ImGui::SliderFloat("Metallic", &metallic, 0, 1))
        mat.set_metallic(metallic);

    float arbitrary0 = mat.get_arbitrary0();
    if (ImGui::InputFloat("Arbitrary0", &arbitrary0))
        mat.set_arbitrary0(arbitrary0);

    switch (mat.get_shading_model())
    {
        case rpcore::RPMaterial::ShadingModel::EMISSIVE_MODEL:
        {
            float intenisty = mat.get_emssive_intentisy();
            if (ImGui::SliderFloat("Intensity", &intenisty, -10, 10))
                mat.set_emissive_intensity(intenisty);
            break;
        }

        case rpcore::RPMaterial::ShadingModel::TRANSPARENT_MODEL:
        {
            bool use_alpha_texture = mat.is_alpha_texture_mode();
            if (ImGui::Checkbox("Use alpha texture", &use_alpha_texture))
            {
                if (use_alpha_texture)
                    mat.set_alpha_texture_mode();
                else
                    mat.set_alpha(1.0f);
            }

            if (!use_alpha_texture)
            {
                float alpha = mat.get_alpha();
                if (ImGui::SliderFloat("Alpha", &alpha, 0, 1))
                    mat.set_alpha(alpha);
            }
            break;
        }

        default:
            break;
    }
}

#define DRAW_COLOR_EDIT(GUI_ID, FUNC_SUFFIX) \
    do { \
        auto color = mat->get_ ## FUNC_SUFFIX (); \
        if (ImGui::ColorEdit4(GUI_ID, &color[0])) \
        { \
            mat->set_ ## FUNC_SUFFIX (color); \
        } \
        \
        ImGui::SameLine(); \
        \
        bool has = mat->has_ ## FUNC_SUFFIX (); \
        if (ImGui::Checkbox("###has_" #FUNC_SUFFIX , &has)) \
        { \
            if (has) \
                mat->set_ ## FUNC_SUFFIX (color); \
            else \
                mat->clear_ ## FUNC_SUFFIX (); \
        } \
    } while (false)

void MaterialWindow::draw_panda_material()
{
    Material* mat = mat_collection_.get_material(current_item_);

    DRAW_COLOR_EDIT("Base Color", base_color);
    DRAW_COLOR_EDIT("Ambient", ambient);
    DRAW_COLOR_EDIT("Diffuse", diffuse);
    DRAW_COLOR_EDIT("Specular", specular);
    DRAW_COLOR_EDIT("Emission", emission);

    {
        float shininess = mat->get_shininess();
        if (ImGui::InputFloat("Shininess", &shininess))
            mat->set_shininess(shininess);
    }

    {
        float value = mat->get_roughness();
        if (ImGui::InputFloat("Roughness", &value))
            mat->set_roughness(value);

        ImGui::SameLine();

        bool has = mat->has_roughness();
        if (ImGui::Checkbox("###has_roughness", &has))
        {
            if (has)
                mat->set_roughness(value);
            else
                mat->set_shininess(mat->get_shininess());       // no clear_roughness
        }
    }

    {
        float value = mat->get_metallic();
        if (ImGui::InputFloat("Metallic", &value))
            mat->set_metallic(value);

        ImGui::SameLine();

        bool has = mat->has_metallic();
        if (ImGui::Checkbox("###has_metallic", &has))
        {
            if (has)
                mat->set_metallic(value);
            else
                mat->clear_metallic();
        }
    }

    {
        float value = mat->get_refractive_index();
        if (ImGui::InputFloat("Refractive Index", &value))
            mat->set_refractive_index(value);

        ImGui::SameLine();

        bool has = mat->has_refractive_index();
        ImGui::Checkbox("###has_refractive_index", &has);
        
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Cannot clear Refractive Index.");
    }

    {
        bool is = mat->get_local();
        if (ImGui::Checkbox("Local", &is))
            mat->set_local(is);
    }

    ImGui::SameLine();

    {
        bool is = mat->get_twoside();
        if (ImGui::Checkbox("Two Side", &is))
            mat->set_twoside(is);
    }
}

#undef DRAW_COLOR_EDIT

}
