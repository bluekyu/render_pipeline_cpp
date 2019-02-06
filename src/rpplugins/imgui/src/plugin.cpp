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

#include "rpplugins/imgui/plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>
#include <boost/filesystem/operations.hpp>

#include <fmt/ostream.h>

#include <imgui.h>

#include <nodePathCollection.h>
#include <mouseButton.h>
#include <colorAttrib.h>
#include <colorBlendAttrib.h>
#include <depthTestAttrib.h>
#include <cullFaceAttrib.h>
#include <scissorAttrib.h>
#include <geomNode.h>
#include <geomTriangles.h>
#include <graphicsWindow.h>
#include <buttonThrower.h>
#include <buttonMap.h>
#include <graphicsWindow.h>

#if defined(__WIN32__) || defined(_WIN32)
#include <WinUser.h>
#include <shellapi.h>
#endif

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rppanda/util/filesystem.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::ImGuiPlugin)

namespace rpplugins {

class ImGuiPlugin::WindowProc : public GraphicsWindowProc
{
public:
    WindowProc(ImGuiPlugin& plugin) : plugin_(plugin)
    {
    }

#if defined(__WIN32__) || defined(_WIN32)
    LONG wnd_proc(GraphicsWindow* graphicsWindow, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override
    {
        switch (msg)
        {
            case WM_DROPFILES:
            {
                HDROP hdrop = (HDROP)wparam;
                POINT pt;
                DragQueryPoint(hdrop, &pt);
                plugin_.dropped_point_ = LVecBase2(static_cast<PN_stdfloat>(pt.x), static_cast<PN_stdfloat>(pt.y));

                const UINT file_count = DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, 0);

                std::vector<wchar_t> buffer;
                plugin_.dropped_files_.clear();
                for (UINT k = 0; k < file_count; ++k)
                {
                    UINT buffer_size = DragQueryFileW(hdrop, k, NULL, 0) + 1;       // #char + \0
                    buffer.resize(buffer_size);
                    UINT ret = DragQueryFileW(hdrop, k, buffer.data(), buffer_size);
                    if (ret)
                    {
                        plugin_.dropped_files_.push_back(Filename::from_os_specific_w(std::wstring(buffer.begin(), buffer.end()-1)));
                    }
                    else
                    {
                        plugin_.error("Failed to query file of dropped.");
                    }
                }

                rppanda::Messenger::get_global_instance()->send(DROPFILES_EVENT_NAME, true);

                break;
            }

            default:
            {
                break;
            }
        }

        return 0;
    }
#endif

private:
    ImGuiPlugin& plugin_;
};

// ************************************************************************************************

rpcore::BasePlugin::RequrieType ImGuiPlugin::require_plugins_;

ImGuiPlugin::ImGuiPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING)
{
}

ImGuiPlugin::~ImGuiPlugin() = default;

rpcore::BasePlugin::RequrieType& ImGuiPlugin::get_required_plugins(void) const
{
    return require_plugins_;
}

void ImGuiPlugin::on_load()
{
    root_ = rpcore::Globals::base->get_pixel_2d().attach_new_node("imgui-root", 1000);

    context_ = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    // Setup back-end capabilities flags
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    const std::string style_setting = boost::any_cast<std::string>(get_setting("style"));
    if (style_setting == "classic")
        ImGui::StyleColorsClassic();
    else if (style_setting == "light")
        ImGui::StyleColorsLight();
    else
        ImGui::StyleColorsDark();

    setup_geom();
    setup_shader();
    setup_font();
    setup_event();
    on_window_resized();

    // ig_loop has process_events and 50 sort.
    add_task(std::bind(&ImGuiPlugin::new_frame_imgui, this, std::placeholders::_1), "ImGuiPlugin::new_frame", 0);
    add_task(std::bind(&ImGuiPlugin::render_imgui, this, std::placeholders::_1), "ImGuiPlugin::render", 40);

    accept(SETUP_CONTEXT_EVENT_NAME, std::bind(&ImGuiPlugin::setup_context, this, std::placeholders::_1));

    // register file drop
#if defined(__WIN32__) || defined(_WIN32)
    enable_file_drop_ = boost::any_cast<bool>(get_setting("os_file_drop"));
    if (enable_file_drop_)
    {
        auto win = rpcore::Globals::base->get_win();
        if (win)
        {
            DragAcceptFiles((HWND)win->get_window_handle()->get_int_handle(), TRUE);
            window_proc_ = std::make_unique<WindowProc>(*this);
            win->add_window_proc(window_proc_.get());
        }
    }
#endif
}

void ImGuiPlugin::on_window_resized()
{
    ImGuiIO& io = ImGui::GetIO();

    const int w = rpcore::Globals::native_resolution[0];
    const int h = rpcore::Globals::native_resolution[1];

    io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
    //io.DisplayFramebufferScale;
}

void ImGuiPlugin::on_unload()
{
#if defined(__WIN32__) || defined(_WIN32)
    if (enable_file_drop_)
    {
        auto win = rpcore::Globals::base->get_win();
        if (win)
        {
            win->remove_window_proc(window_proc_.get());
            window_proc_.reset();
            DragAcceptFiles((HWND)win->get_window_handle()->get_int_handle(), FALSE);
        }
    }
#endif

    remove_all_tasks();

    ImGui::DestroyContext();
    context_ = nullptr;
}

void ImGuiPlugin::setup_context(const Event* ev)
{
    const auto num_parameters = ev->get_num_parameters();
    if (num_parameters != 1)
    {
        error(fmt::format("Invalid number of task: {}", num_parameters));
        return;
    }

    auto param = ev->get_parameter(0);
    if (!param.is_typed_ref_count())
    {
        error(fmt::format("Invalid type of parameter."));
        return;
    }

    auto ptr = param.get_typed_ref_count_value();
    if (!ptr->is_of_type(rppanda::FunctionalTask::get_class_type()))
    {
        error(fmt::format("Type of parameter is NOT rppanda::FunctionalTask"));
        return;
    }

    auto task = DCAST(rppanda::FunctionalTask, ptr);
    task->set_user_data(std::shared_ptr<ImGuiContext>(get_context(), [](auto) {}));
    add_task(task);
}

void ImGuiPlugin::setup_geom()
{
    ImGuiIO& io = ImGui::GetIO();

    PT(GeomVertexArrayFormat) array_format = new GeomVertexArrayFormat(
        InternalName::get_vertex(), 4, Geom::NT_stdfloat, Geom::C_point,
        InternalName::get_color(), 1, Geom::NT_packed_dabc, Geom::C_color
    );

    vformat_ = GeomVertexFormat::register_format(new GeomVertexFormat(array_format));

    root_.set_state(RenderState::make(
        ColorAttrib::make_vertex(),
        ColorBlendAttrib::make(ColorBlendAttrib::M_add, ColorBlendAttrib::O_incoming_alpha, ColorBlendAttrib::O_one_minus_incoming_alpha),
        DepthTestAttrib::make(DepthTestAttrib::M_none),
        CullFaceAttrib::make(CullFaceAttrib::M_cull_none)
    ));
}

NodePath ImGuiPlugin::create_geomnode(const GeomVertexData* vdata)
{
    ImGuiIO& io = ImGui::GetIO();

    PT(GeomTriangles) prim = new GeomTriangles(GeomEnums::UsageHint::UH_stream);

    static_assert(
        sizeof(ImDrawIdx) == sizeof(uint16_t) ||
        sizeof(ImDrawIdx) == sizeof(uint32_t),
        "Type of ImDrawIdx is not uint16_t or uint32_t. Update below code!"
        );
    if (sizeof(ImDrawIdx) == sizeof(uint16_t))
        prim->set_index_type(GeomEnums::NumericType::NT_uint16);
    else if (sizeof(ImDrawIdx) == sizeof(uint32_t))
        prim->set_index_type(GeomEnums::NumericType::NT_uint32);

    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    PT(GeomNode) geom_node = new GeomNode("imgui-geom");
    geom_node->add_geom(geom, RenderState::make_empty());

    return NodePath(geom_node);
}

void ImGuiPlugin::setup_font()
{
    ImGuiIO& io = ImGui::GetIO();

    auto default_font_path = rppanda::convert_path(Filename(boost::any_cast<std::string>(get_setting("default_font_path"))));
    if (boost::filesystem::exists(default_font_path))
    {
        const float font_size = boost::any_cast<float>(get_setting("default_font_size"));
        io.Fonts->AddFontFromFileTTF(default_font_path.generic_string().c_str(), font_size);
    }
    else
    {
        error(fmt::format("Failed to find font: {}", default_font_path));
        io.Fonts->AddFontDefault();
    }

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    font_texture_ = new Texture("imgui-font-texture");
    font_texture_->setup_2d_texture(width, height, Texture::ComponentType::T_unsigned_byte, Texture::Format::F_red);
    font_texture_->set_minfilter(SamplerState::FilterType::FT_linear);
    font_texture_->set_magfilter(SamplerState::FilterType::FT_linear);

    PTA_uchar ram_image = font_texture_->make_ram_image();
    std::memcpy(ram_image.p(), pixels, width * height * sizeof(decltype(*pixels)));

    io.Fonts->TexID = font_texture_.p();
}

void ImGuiPlugin::setup_shader()
{
    root_.set_shader(rpcore::RPLoader::load_shader({ get_shader_resource("imgui.vert.glsl"), get_shader_resource("imgui.frag.glsl") }));
}

void ImGuiPlugin::setup_event()
{
    ImGuiIO& io = ImGui::GetIO();

    button_map_ = rpcore::Globals::base->get_win()->get_keyboard_map();

    io.KeyMap[ImGuiKey_Tab] = KeyboardButton::tab().get_index();
    io.KeyMap[ImGuiKey_LeftArrow] = KeyboardButton::left().get_index();
    io.KeyMap[ImGuiKey_RightArrow] = KeyboardButton::right().get_index();
    io.KeyMap[ImGuiKey_UpArrow] = KeyboardButton::up().get_index();
    io.KeyMap[ImGuiKey_DownArrow] = KeyboardButton::down().get_index();
    io.KeyMap[ImGuiKey_PageUp] = KeyboardButton::page_up().get_index();
    io.KeyMap[ImGuiKey_PageDown] = KeyboardButton::page_down().get_index();
    io.KeyMap[ImGuiKey_Home] = KeyboardButton::home().get_index();
    io.KeyMap[ImGuiKey_End] = KeyboardButton::end().get_index();
    io.KeyMap[ImGuiKey_Insert] = KeyboardButton::insert().get_index();
    io.KeyMap[ImGuiKey_Delete] = KeyboardButton::del().get_index();
    io.KeyMap[ImGuiKey_Backspace] = KeyboardButton::backspace().get_index();
    io.KeyMap[ImGuiKey_Space] = KeyboardButton::space().get_index();
    io.KeyMap[ImGuiKey_Enter] = KeyboardButton::enter().get_index();
    io.KeyMap[ImGuiKey_Escape] = KeyboardButton::escape().get_index();
    io.KeyMap[ImGuiKey_A] = KeyboardButton::ascii_key('a').get_index();
    io.KeyMap[ImGuiKey_C] = KeyboardButton::ascii_key('c').get_index();
    io.KeyMap[ImGuiKey_V] = KeyboardButton::ascii_key('v').get_index();
    io.KeyMap[ImGuiKey_X] = KeyboardButton::ascii_key('x').get_index();
    io.KeyMap[ImGuiKey_Y] = KeyboardButton::ascii_key('y').get_index();
    io.KeyMap[ImGuiKey_Z] = KeyboardButton::ascii_key('z').get_index();

    if (auto bt = rpcore::Globals::base->get_button_thrower())
    {
        ButtonThrower* bt_node = DCAST(ButtonThrower, bt.node());
        std::string ev_name;

        ev_name = bt_node->get_button_down_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-button-down";
            bt_node->set_button_down_event(ev_name);
        }
        accept(ev_name, [this](const Event* ev) { on_button_down_or_up(ev, true); });

        ev_name = bt_node->get_button_up_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-button-up";
            bt_node->set_button_up_event(ev_name);
        }
        accept(ev_name, [this](const Event* ev) { on_button_down_or_up(ev, false); });

        ev_name = bt_node->get_keystroke_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-keystroke";
            bt_node->set_keystroke_event(ev_name);
        }
        accept(ev_name, std::bind(&ImGuiPlugin::on_keystroke, this, std::placeholders::_1));
    }
}

AsyncTask::DoneStatus ImGuiPlugin::new_frame_imgui(rppanda::FunctionalTask*)
{
    if (root_.is_hidden())
        return AsyncTask::DS_cont;

    update_imgui();

    ImGui::NewFrame();

    throw_event_directly(*EventHandler::get_global_event_handler(), NEW_FRAME_EVENT_NAME);

    return AsyncTask::DS_cont;
}

void ImGuiPlugin::update_imgui()
{
    static const int MOUSE_DEVICE_INDEX = 0;

    ImGuiIO& io = ImGui::GetIO();

    io.DeltaTime = static_cast<float>(rpcore::Globals::clock->get_dt());

    auto window = rpcore::Globals::base->get_win();
    if (window && window->is_of_type(GraphicsWindow::get_class_type()))
    {
        const auto& mouse = window->get_pointer(MOUSE_DEVICE_INDEX);
        if (mouse.get_in_window())
        {
            if (io.WantSetMousePos)
            {
                window->move_pointer(MOUSE_DEVICE_INDEX, io.MousePos.x, io.MousePos.y);
            }
            else
            {
                io.MousePos.x = static_cast<float>(mouse.get_x());
                io.MousePos.y = static_cast<float>(mouse.get_y());
            }
        }
        else
        {
            io.MousePos.x = -FLT_MAX;
            io.MousePos.y = -FLT_MAX;
        }
    }
}

AsyncTask::DoneStatus ImGuiPlugin::render_imgui(rppanda::FunctionalTask* task)
{
    if (root_.is_hidden())
        return AsyncTask::DS_cont;

    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    const float fb_width = io.DisplaySize.x * io.DisplayFramebufferScale.x;
    const float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;

    auto draw_data = ImGui::GetDrawData();
    //draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    auto npc = root_.get_children();
    for (int k = 0, k_end = npc.get_num_paths(); k < k_end; ++k)
        npc.get_path(k).detach_node();

    for (int k = 0; k < draw_data->CmdListsCount; ++k)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[k];

        if (!(k < geom_data_.size()))
        {
            geom_data_.push_back({
                new GeomVertexData("imgui-vertex-" + std::to_string(k), vformat_, GeomEnums::UsageHint::UH_stream),
                {}
            });
        }

        auto& geom_list = geom_data_[k];

        auto vertex_handle = geom_list.vdata->modify_array_handle(0);
        if (vertex_handle->get_num_rows() < cmd_list->VtxBuffer.Size)
            vertex_handle->unclean_set_num_rows(cmd_list->VtxBuffer.Size);

        std::memcpy(
            vertex_handle->get_write_pointer(),
            reinterpret_cast<const unsigned char*>(cmd_list->VtxBuffer.Data),
            cmd_list->VtxBuffer.Size * sizeof(decltype(cmd_list->VtxBuffer)::value_type));

        auto idx_buffer_data = cmd_list->IdxBuffer.Data;
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
        {
            const ImDrawCmd* draw_cmd = &cmd_list->CmdBuffer[cmd_i];
            auto elem_count = static_cast<int>(draw_cmd->ElemCount);

            if (!(cmd_i < geom_list.nodepaths.size()))
                geom_list.nodepaths.push_back(create_geomnode(geom_list.vdata));

            NodePath np = geom_list.nodepaths[cmd_i];
            np.reparent_to(root_);

            auto gn = DCAST(GeomNode, np.node());

            auto index_handle = gn->modify_geom(0)->modify_primitive(0)->modify_vertices(elem_count)->modify_handle();
            if (index_handle->get_num_rows() < elem_count)
                index_handle->unclean_set_num_rows(elem_count);

            std::memcpy(
                index_handle->get_write_pointer(),
                reinterpret_cast<const unsigned char*>(idx_buffer_data),
                elem_count * sizeof(decltype(cmd_list->IdxBuffer)::value_type));
            idx_buffer_data += elem_count;

            CPT(RenderState) state = RenderState::make(ScissorAttrib::make(
                draw_cmd->ClipRect.x / fb_width,
                draw_cmd->ClipRect.z / fb_width,
                1 - draw_cmd->ClipRect.w / fb_height,
                1 - draw_cmd->ClipRect.y / fb_height));

            if (draw_cmd->TextureId)
                state = state->add_attrib(TextureAttrib::make(static_cast<Texture*>(draw_cmd->TextureId)));

            gn->set_geom_state(0, state);
        }
    }

    return AsyncTask::DS_cont;
}

void ImGuiPlugin::on_button_down_or_up(const Event* ev, bool down)
{
    const auto& key_name = ev->get_parameter(0).get_string_value();
    const auto& button = ButtonRegistry::ptr()->get_button(key_name);

    if (button == ButtonHandle::none())
        return;

    ImGuiIO& io = ImGui::GetIO();
    if (MouseButton::is_mouse_button(button))
    {
        if (button == MouseButton::one())
        {
            io.MouseDown[0] = down;
        }
        else if (button == MouseButton::three())
        {
            io.MouseDown[1] = down;
        }
        else if (button == MouseButton::two())
        {
            io.MouseDown[2] = down;
        }
        else if (button == MouseButton::four())
        {
            io.MouseDown[3] = down;
        }
        else if (button == MouseButton::five())
        {
            io.MouseDown[4] = down;
        }
        else if (down)
        {
            if (button == MouseButton::wheel_up())
                io.MouseWheel += 1;
            else if (button == MouseButton::wheel_down())
                io.MouseWheel -= 1;
            else if (button == MouseButton::wheel_right())
                io.MouseWheelH += 1;
            else if (button == MouseButton::wheel_left())
                io.MouseWheelH -= 1;
        }
    }
    else
    {
        io.KeysDown[button.get_index()] = down;

        if (button == KeyboardButton::control())
            io.KeyCtrl = down;
        else if (button == KeyboardButton::shift())
            io.KeyShift = down;
        else if (button == KeyboardButton::alt())
            io.KeyAlt = down;
        else if (button == KeyboardButton::meta())
            io.KeySuper = down;
    }
}

void ImGuiPlugin::on_keystroke(const Event* ev)
{
    wchar_t keycode = ev->get_parameter(0).get_wstring_value()[0];
    if (keycode < 0 || keycode >= (std::numeric_limits<ImWchar>::max)())
        return;

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(keycode);
}

}
