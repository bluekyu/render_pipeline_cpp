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

#include "nodepath_window.hpp"

#include <fmt/ostream.h>

#include <paramNodePath.h>
#include <showBoundsEffect.h>
#include <cullFaceAttrib.h>
#include <depthTestAttrib.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

#include "rpplugins/rpstat/plugin.hpp"

#include "imgui/imgui_stdlib.h"
#include "scenegraph_window.hpp"
#include "file_dialog.hpp"

namespace rpplugins {

NodePathWindow::NodePathWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline) : WindowInterface(plugin, pipeline, "NodePath Window", "###NodePath")
{
    window_flags_ |= ImGuiWindowFlags_MenuBar;

    file_dialog_ = std::make_unique<FileDialog>(plugin_, FileDialog::OperationFlag::write);

    accept(
        ScenegraphWindow::NODE_SELECTED_EVENT_NAME,
        [this](const Event* ev) { set_nodepath(DCAST(ParamNodePath, ev->get_parameter(0).get_ptr())->get_value()); }
    );
}

NodePathWindow::~NodePathWindow() = default;

void NodePathWindow::draw_contents()
{
    enum class MenuID : int
    {
        None,
        Tools_Flatten_Light,
        Tools_Flatten_Medium,
        Tools_Flatten_Strong,
    };
    static MenuID menu_selected = MenuID::None;

    const bool np_exist = !np_.is_empty();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Tools", np_exist))
        {
            bool visible = !np_.is_hidden();
            if (ImGui::MenuItem("Visible", nullptr, &visible))
            {
                if (visible)
                    np_.show();
                else
                    np_.hide();
            }

            bool bounds_visible = np_.get_effect(ShowBoundsEffect::get_class_type()) != nullptr;
            if (ImGui::MenuItem("Show Tight Bounds", nullptr, &bounds_visible))
            {
                if (bounds_visible)
                    np_.show_tight_bounds();
                else
                    np_.hide_bounds();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Show Material", nullptr, false, np_exist))
            {
                send_show_event("###Material");
                throw_event(ScenegraphWindow::NODE_SELECTED_EVENT_NAME, EventParameter(new ParamNodePath(np_)));
            }

            if (ImGui::MenuItem("New Material", nullptr, false, np_exist))
                np_.set_material(rpcore::RPMaterial().get_material());

            if (ImGui::MenuItem("Clear Material", nullptr, false, np_exist && np_.has_material()))
                np_.clear_material();

            ImGui::Separator();

            if (ImGui::BeginMenu("Flatten"))
            {
                if (ImGui::MenuItem("Light"))
                    menu_selected = MenuID::Tools_Flatten_Light;

                if (ImGui::MenuItem("Medium"))
                    menu_selected = MenuID::Tools_Flatten_Medium;

                if (ImGui::MenuItem("Strong"))
                    menu_selected = MenuID::Tools_Flatten_Strong;

                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Write Bam File"))
                file_dialog_->open();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (!np_exist)
        return;

    ui_write_bam();

    if (menu_selected == MenuID::Tools_Flatten_Light)
        ImGui::OpenPopup("Light...");
    ui_flatten("Light...");

    if (menu_selected == MenuID::Tools_Flatten_Medium)
        ImGui::OpenPopup("Medium...");
    ui_flatten("Medium...");

    if (menu_selected == MenuID::Tools_Flatten_Strong)
        ImGui::OpenPopup("Strong...");
    ui_flatten("Strong...");

    ui_name();
    ui_transform();
    ui_render_mode();
    ui_camera_mask();
    ui_tag();

    ImGui::Separator();

    ui_cull_face();
    ui_depth_test();
    ui_transparency();

    ImGui::Separator();

    ui_effect();

    menu_selected = MenuID::None;
}

void NodePathWindow::set_nodepath(NodePath np)
{
    np_ = np;
    tag_index_ = -1;

    if (np_)
        name_buffer_ = np_.get_name();
}

void NodePathWindow::ui_write_bam()
{
    const auto& accepted = file_dialog_->draw();
    if (!(accepted && *accepted))
        return;

    const auto& fname = file_dialog_->get_filename();
    if (!fname.empty())
    {
        if (!np_.write_bam_file(fname))
        {
            plugin_.error(fmt::format("Failed to write Bam file: {}", std::string(fname)));
        }
    }
}

void NodePathWindow::ui_name()
{
    if (ImGui::InputText("Name", &name_buffer_, ImGuiInputTextFlags_EnterReturnsTrue))
        np_.set_name(name_buffer_);
}

void NodePathWindow::ui_transform()
{
    if (ImGui::CollapsingHeader("Transforms", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
    {
        enum TransformMode : int
        {
            LOCAL = 0,
            GLOBAL,
            OTHER
        };

        static NodePath selected_other;
        static int transform_mode = TransformMode::LOCAL;

        ImGui::RadioButton("Local", &transform_mode, TransformMode::LOCAL); ImGui::SameLine();

        NodePath other;
        if (ImGui::RadioButton("Global", &transform_mode, TransformMode::GLOBAL))
            other = rpcore::Globals::render;
        ImGui::SameLine();

        // TODO: implement Scenegraph drag & drop
        ImGui::RadioButton(
            fmt::format("Other{}###OtherRadioButton", selected_other ? " : " + selected_other.node()->get_name() : "").c_str(),
            &transform_mode,
            TransformMode::OTHER);

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Drag-&-drop NodePath OR copy-&-paste it");

        // drag & drop target for NodePath
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodePath"))
            {
                auto source_np = *reinterpret_cast<NodePath*>(payload->Data);
                selected_other = source_np;
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem())
        {
            auto copied_np = plugin_.get_copied_nodepath();
            if (copied_np)
            {
                if (ImGui::Selectable("Paste"))
                    selected_other = copied_np;
            }
            else
            {
                ImGui::TextDisabled("Paste");
            }

            ImGui::EndPopup();
        }

        if (transform_mode == TransformMode::OTHER)
            other = selected_other;

        static bool show_matrix = false;
        if (show_matrix)
        {
            LMatrix4f mat = transform_mode == TransformMode::LOCAL ? np_.get_mat() : np_.get_mat(other);

            bool edited = false;
            edited |= ImGui::InputFloat4("Row 0", &mat(0, 0), 3, ImGuiInputTextFlags_EnterReturnsTrue);

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("R-click to show Position/Rotation/Scale");
                if (ImGui::IsMouseClicked(1))
                    show_matrix = false;
            }

            edited |= ImGui::InputFloat4("Row 1", &mat(1, 0), 3, ImGuiInputTextFlags_EnterReturnsTrue);
            edited |= ImGui::InputFloat4("Row 2", &mat(2, 0), 3, ImGuiInputTextFlags_EnterReturnsTrue);
            edited |= ImGui::InputFloat4("Row 3", &mat(3, 0), 3, ImGuiInputTextFlags_EnterReturnsTrue);

            if (edited)
            {
                if (transform_mode == TransformMode::LOCAL)
                    np_.set_mat(mat);
                else
                    np_.set_mat(other, mat);
            }
        }
        else
        {
            LVecBase3f pos = transform_mode == TransformMode::LOCAL ? np_.get_pos() : np_.get_pos(other);
            if (ImGui::InputFloat3("Position", &pos[0], 3, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (transform_mode == TransformMode::LOCAL)
                    np_.set_pos(pos);
                else
                    np_.set_pos(other, pos);
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("R-click to show matrix");
                if (ImGui::IsMouseClicked(1))
                    show_matrix = true;
            }

            {
                static bool show_hpr = true;

                if (show_hpr)
                {
                    LVecBase3f hpr = transform_mode == TransformMode::LOCAL ? np_.get_hpr() : np_.get_hpr(other);
                    if (ImGui::InputFloat3("HPR", &hpr[0], 3, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (transform_mode == TransformMode::LOCAL)
                            np_.set_hpr(hpr);
                        else
                            np_.set_hpr(other, hpr);
                    }
                }
                else
                {
                    auto quat = transform_mode == TransformMode::LOCAL ? np_.get_quat() : np_.get_quat(other);
                    if (ImGui::InputFloat4("Quat (rijk)", &quat[0], 3, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (transform_mode == TransformMode::LOCAL)
                            np_.set_quat(quat);
                        else
                            np_.set_quat(other, quat);
                    }
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("R-click to show %s", show_hpr ? "Quaternion" : "HPR");

                    if (ImGui::IsMouseClicked(1))
                        show_hpr = !show_hpr;;
                }
            }

            LVecBase3f scale = transform_mode == TransformMode::LOCAL ? np_.get_scale() : np_.get_scale(other);
            if (ImGui::InputFloat3("Scale", &scale[0], 3, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (transform_mode == TransformMode::LOCAL)
                    np_.set_scale(scale);
                else
                    np_.set_scale(other, scale);
            }
        }
    }
}

void NodePathWindow::ui_render_mode()
{
    if (ImGui::CollapsingHeader("Render Mode"))
    {
        static const char* render_modes[] = { "unchanged", "filled", "wireframe", "point", "filled flat", "filled wireframe" };
        int render_mode_index = np_.get_render_mode();
        bool changed = ImGui::ListBox("Render Mode\n(single select)", &render_mode_index, render_modes, std::extent<decltype(render_modes)>::value, 5);

        switch (render_mode_index)
        {
            case RenderModeAttrib::Mode::M_unchanged:
            {
                np_.clear_render_mode();
                break;
            }

            case RenderModeAttrib::Mode::M_filled:
            {
                if (changed)
                    np_.set_render_mode_filled();
                break;
            }

            case RenderModeAttrib::Mode::M_wireframe:
            {
                if (changed)
                    np_.set_render_mode_wireframe();
                break;
            }

            case RenderModeAttrib::Mode::M_point:
            {
                float thickness = np_.get_render_mode_thickness();
                changed |= ImGui::InputFloat("Thickness", &thickness);

                if (changed)
                    np_.set_render_mode(RenderModeAttrib::Mode::M_point, thickness);
                break;
            }

            case RenderModeAttrib::Mode::M_filled_flat:
            {
                float thickness = np_.get_render_mode_thickness();
                changed |= ImGui::InputFloat("Thickness", &thickness);

                if (changed)
                    np_.set_render_mode(RenderModeAttrib::Mode::M_filled_flat, thickness);
                break;
            }

            case RenderModeAttrib::Mode::M_filled_wireframe:
            {
                LColor wireframe_color(1);
                if (auto attrib = np_.get_attrib(RenderModeAttrib::get_class_type()))
                    wireframe_color = DCAST(RenderModeAttrib, attrib)->get_wireframe_color();
                changed |= ImGui::ColorEdit4("Wireframe Color", &wireframe_color[0]);

                if (changed)
                    np_.set_render_mode_filled_wireframe(wireframe_color);
                break;
            }
        }
    }
}

void NodePathWindow::ui_camera_mask()
{
    if (!ImGui::CollapsingHeader("Camera Mask"))
        return;

    auto node = np_.node();
    const auto show_mask = node->get_draw_show_mask();
    const auto control_mask = node->get_draw_control_mask();

    ImGui::LabelText("Show Mask", "%X", show_mask);
    ImGui::LabelText("Control Mask", "%X", control_mask);

    static ImU32 cam_mask = 0;
    ImGui::InputScalar("Camera Mask###NodePathCameraMask", ImGuiDataType_U32, &cam_mask, nullptr, nullptr, "%08X", ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Show###NodePathShow"))
        np_.show(cam_mask);

    ImGui::SameLine();

    if (ImGui::Button("Show Through###NodePathShowThrough"))
        np_.show_through(cam_mask);

    ImGui::SameLine();

    if (ImGui::Button("Hide###NodePathHide"))
        np_.hide(cam_mask);

    ImGui::SameLine();

    ImGui::Button("Hidden?###NodePathHidden");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip(np_.is_hidden(cam_mask) ? "Hidden" : "Shown");
}

void NodePathWindow::ui_tag()
{
    if (!ImGui::CollapsingHeader("Tags"))
        return;

    static vector_string keys;
    static auto getter = [](void* data, int idx, const char** out_text) {
        const auto& _keys = *reinterpret_cast<decltype(keys)*>(data);
        *out_text = _keys[idx].c_str();
        return true;
    };

    keys.clear();
    np_.get_tag_keys(keys);

    ImGui::Combo("Keys", &tag_index_, getter, &keys, static_cast<int>(keys.size()));

    ImGui::LabelText("Value", "%s", tag_index_ != -1 ? np_.get_tag(keys[tag_index_]).c_str() : "");
}

void NodePathWindow::ui_cull_face()
{
    static const char* items[] = { "Cull None (Two Sided)", "Cull Clockwise (No Two Sided)", "Cull Counter Clockwise", "Unchanged", "Clear" };
    constexpr size_t items_size = std::extent<decltype(items)>::value;
    constexpr size_t last_item_index = items_size - 1;
    int item_current;
    static_assert(CullFaceAttrib::Mode::M_cull_unchanged == items_size - 2, "API is changed! Update this code.");

    auto node = np_.node();
    if (node->has_attrib(CullFaceAttrib::get_class_slot()))
        item_current = static_cast<int>(DCAST(CullFaceAttrib, node->get_attrib(CullFaceAttrib::get_class_slot()))->get_actual_mode());
    else
        item_current = last_item_index;

    if (ImGui::Combo("Cull Face", &item_current, items, items_size))
    {
        if (item_current == last_item_index)
            np_.clear_two_sided();
        else
            node->set_attrib(CullFaceAttrib::make(static_cast<CullFaceAttrib::Mode>(item_current)));
    }
}

void NodePathWindow::ui_depth_test()
{
    static const char* items[] = { "None (No Test)", "Never", "Less (Test)", "Equal", "Less Equal", "Greater", "Not Equal", "Greater Equal", "Always", "Clear" };
    constexpr size_t items_size = std::extent<decltype(items)>::value;
    constexpr size_t last_item_index = items_size - 1;
    int item_current;
    static_assert(RenderAttrib::PandaCompareFunc::M_always == items_size - 2, "API is changed! Update this code.");

    auto node = np_.node();
    if (node->has_attrib(DepthTestAttrib::get_class_slot()))
        item_current = static_cast<int>(DCAST(DepthTestAttrib, node->get_attrib(DepthTestAttrib::get_class_slot()))->get_mode());
    else
        item_current = last_item_index;

    if (ImGui::Combo("Depth Test", &item_current, items, items_size))
    {
        if (item_current == last_item_index)
            np_.clear_depth_test();
        else
            node->set_attrib(DepthTestAttrib::make(static_cast<RenderAttrib::PandaCompareFunc>(item_current)));
    }
}

void NodePathWindow::ui_transparency()
{
    static const char* items[] = { "None", "Alpha", "Premultiplied Alpha", "Multisample", "Multisample Mask", "Binary", "Dual", "Clear" };
    constexpr size_t items_size = std::extent<decltype(items)>::value;
    constexpr size_t last_item_index = items_size - 1;
    int item_current;
    static_assert(TransparencyAttrib::Mode::M_dual == items_size - 2, "API is changed! Update this code.");

    auto node = np_.node();
    if (node->has_attrib(TransparencyAttrib::get_class_slot()))
        item_current = static_cast<int>(DCAST(TransparencyAttrib, node->get_attrib(TransparencyAttrib::get_class_slot()))->get_mode());
    else
        item_current = last_item_index;

    if (ImGui::Combo("Transparency", &item_current, items, items_size))
    {
        if (item_current == last_item_index)
            np_.clear_transparency();
        else
            node->set_attrib(TransparencyAttrib::make(static_cast<TransparencyAttrib::Mode>(item_current)));
    }
}

void NodePathWindow::ui_flatten(const char* popup_id)
{
    if (!ImGui::BeginPopupModal(popup_id, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    ImGui::Text("Selected NodePath will be flatten\nThis operation cannot be undone!\n\n");
    ImGui::Separator();

    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
        std::string name(popup_id);
        if (name.find("Light") != std::string::npos)
            np_.flatten_light();
        else if (name.find("Medium") != std::string::npos)
            np_.flatten_medium();
        else if (name.find("Strong") != std::string::npos)
            np_.flatten_strong();

        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0)))
    {
        ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
}

void NodePathWindow::ui_effect()
{
    if (!ImGui::CollapsingHeader("RP Effect"))
        return;

    if (pipeline_.has_effect(np_))
    {
        const auto& effect = pipeline_.get_effect(np_);

        bool changed = false;
        const auto& fname = effect.first.first;
        const auto& option = effect.first.second;
        int sort = effect.second;

        ImGui::LabelText("File", fname.to_os_generic().c_str());

        // TODO: option

        changed = changed || ImGui::InputInt("Sort", &sort);
        if (changed)
            pipeline_.set_effect(np_, fname, option, sort);

        if (ImGui::Button("Clear", ImVec2(120, 0)))
            pipeline_.clear_effect(np_);
    }
    else
    {
        ImGui::Text("Empty Effect");
    }
}

}
