#include "rpcore/gui/buffer_viewer.hpp"

#include <regex>
#include <unordered_set>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/gui/direct_scrolled_frame.hpp"
#include "render_pipeline/rppanda/gui/direct_scroll_bar.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"
#include "render_pipeline/rppanda/gui/direct_check_box.hpp"

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rpcore/gui/labeled_checkbox.hpp"
#include "render_pipeline/rpcore/render_target.hpp"
#include "render_pipeline/rpcore/image.hpp"

#include "rpcore/gui/texture_preview.hpp"
#include "rpcore/util/display_shader_builder.hpp"

namespace rpcore {

BufferViewer::BufferViewer(RenderPipeline* pipeline, NodePath parent): DraggableWindow(1400, 800, "Buffer- and Image-Browser", parent), _pipeline(pipeline)
{
    create_components();

    _tex_preview = std::make_shared<TexturePreview>(_pipeline, parent);
    _tex_preview->hide();

    hide();
}

void BufferViewer::toggle(void)
{
    if (_visible)
    {
        remove_components();
        hide();
    }
    else
    {
        perform_update();
        show();
    }
}

std::vector<BufferViewer::EntryType> BufferViewer::get_entries(void) const
{
    std::vector<BufferViewer::EntryType> entries;

    for (const auto& target: RenderTarget::REGISTERED_TARGETS)
        entries.push_back({target, EntryID::RENDER_TARGET});

    for (const auto& image: Image::REGISTERED_IMAGES)
        entries.push_back({image, EntryID::TEXTURE});

    return entries;
}

std::pair<size_t, int> BufferViewer::get_stage_information(void) const
{
    int count = 0;
    size_t memory = 0;

    const auto& entries = get_entries();
    for (const auto& entry: entries)
    {
        if (entry.second == EntryID::TEXTURE)
        {
            memory += reinterpret_cast<Image*>(entry.first)->get_texture()->estimate_texture_memory();
            ++count;
        }
        else if (entry.second == EntryID::RENDER_TARGET)
        {
            for (const auto& id_target: reinterpret_cast<RenderTarget*>(entry.first)->get_targets())
            {
                memory += id_target.second->estimate_texture_memory();
                ++count;
            }
        }
        else
        {
            warn(std::string("Unkown type: ") + std::to_string(int(entry.second)));
        }
    }

    return {memory, count};
}

void BufferViewer::create_components(void)
{
    DraggableWindow::create_components();

    auto content_frame_options = std::make_shared<rppanda::DirectScrolledFrame::Options>();
    content_frame_options->frame_size = LVecBase4f(0, _width - 15, 0, _height - 70);
    content_frame_options->canvas_size = LVecBase4f(0, _width - 80, 0, _scroll_height);
    content_frame_options->auto_hide_scroll_bars = false;
    content_frame_options->scroll_bar_width = 12.0f;
    content_frame_options->frame_color = LColorf(0, 0, 0, 0);
    content_frame_options->vertical_scroll_options->relief = rppanda::FLAT;
    content_frame_options->vertical_scroll_options->inc_button_options->relief = rppanda::FLAT;
    content_frame_options->vertical_scroll_options->dec_button_options->relief = rppanda::FLAT;
    content_frame_options->vertical_scroll_options->thumb_options->relief = rppanda::FLAT;
    content_frame_options->vertical_scroll_options->frame_color = LColorf(0.05, 0.05, 0.05, 1);
    content_frame_options->vertical_scroll_options->thumb_options->frame_color = LColorf(0.8, 0.8, 0.8, 1);
    content_frame_options->vertical_scroll_options->inc_button_options->frame_color = LColorf(0.6, 0.6, 0.6, 1);
    content_frame_options->vertical_scroll_options->dec_button_options->frame_color = LColorf(0.6, 0.6, 0.6, 1);
    content_frame_options->horizontal_scroll_options->relief = PGFrameStyle::Type(0);
    content_frame_options->horizontal_scroll_options->frame_color = LColorf(0);
    content_frame_options->horizontal_scroll_options->thumb_options->relief = PGFrameStyle::Type(0);
    content_frame_options->horizontal_scroll_options->inc_button_options->relief = PGFrameStyle::Type(0);
    content_frame_options->horizontal_scroll_options->dec_button_options->relief = PGFrameStyle::Type(0);
    content_frame_options->pos = LVecBase3f(0, 1, -_height);
    _content_frame = new rppanda::DirectScrolledFrame(_node, content_frame_options);

    _content_node = _content_frame->get_canvas().attach_new_node("BufferComponents");
    _content_node.set_scale(1, 1, -1);
    _content_node.set_z(_scroll_height);

    LabeledCheckbox::Parameters labeled_checkbox_params;
    labeled_checkbox_params.parent = _node;
    labeled_checkbox_params.x = 10;
    labeled_checkbox_params.y = 43;
    labeled_checkbox_params.callback = std::bind(&BufferViewer::set_show_images, this, std::placeholders::_1, std::placeholders::_2);
    labeled_checkbox_params.checked = false;
    labeled_checkbox_params.text = "Display image resources";
    labeled_checkbox_params.text_color = LVecBase3f(0.4f);
    labeled_checkbox_params.expand_width = 330;
    _chb_show_images = std::make_shared<LabeledCheckbox>(labeled_checkbox_params);
}

void BufferViewer::set_show_images(bool arg, void* extra_args)
{
    _display_images = arg;
    perform_update();
}

void BufferViewer::set_scroll_height(int height)
{
    _scroll_height = height;
    _content_frame->set_canvas_size(LVecBase4f(0, _width - 80.0f, 0, _scroll_height));
    _content_node.set_z(_scroll_height);
}

void BufferViewer::remove_components(void)
{
    frame_hovers_.clear();

    frame_click_data.clear();

    _content_node.node()->remove_all_children();

    _tex_preview->hide();
}

void BufferViewer::perform_update(void)
{
    // Collect texture stages
    _stages.clear();
    auto entries = get_entries();
    std::sort(entries.begin(), entries.end(), [&](const EntryType& lhs, const EntryType& rhs) {
        const int lhs_sort = lhs.second == EntryID::RENDER_TARGET ?
            reinterpret_cast<RenderTarget*>(lhs.first)->get_sort().get() :
            reinterpret_cast<Image*>(lhs.first)->get_sort();

        const int rhs_sort = rhs.second == EntryID::RENDER_TARGET ?
            reinterpret_cast<RenderTarget*>(rhs.first)->get_sort().get() :
            reinterpret_cast<Image*>(rhs.first)->get_sort();

        return lhs_sort < rhs_sort;
    });

    for (const auto& entry: entries)
    {
        switch (entry.second)
        {
        case EntryID::TEXTURE:
            {
                if (_display_images)
                    _stages.push_back({reinterpret_cast<Image*>(entry.first)->get_texture(), entry.second});
                break;
            }
        case EntryID::RENDER_TARGET:
            {
                for (const auto& key_val: reinterpret_cast<RenderTarget*>(entry.first)->get_targets())
                    _stages.push_back({key_val.second.p(), entry.second});
                break;
            }
        default:
            warn("Unrecognized instance!");
        }
    }

    render_stages();
}

void BufferViewer::on_texture_hovered(const Event* func, void* data)
{
    reinterpret_cast<rppanda::DirectFrame*>(data)->set_frame_color(LColorf(0, 0, 0, 0.1f));
}

void BufferViewer::on_texture_blurred(const Event* func, void* data)
{
    reinterpret_cast<rppanda::DirectFrame*>(data)->set_frame_color(LColorf(0, 0, 0, 0.0f));
}

void BufferViewer::on_texture_clicked(const Event* func, void* data)
{
    FrameClickDataType* self_tex = reinterpret_cast<FrameClickDataType*>(data);
    self_tex->self->_tex_preview->present(self_tex->tex);
}

void BufferViewer::render_stages(void)
{
    remove_components();
    const int entries_per_row = 6;
    const float aspect = Globals::native_resolution.get_y() / float(Globals::native_resolution.get_x());
    const int entry_width = 235;
    const int entry_height = (entry_width - 20) * aspect + 55;

    // Store already processed images
    std::unordered_set<Texture*> processed;
    int index = -1;

    // Iterate over all stages
    frame_click_data.clear();
    for (const auto& stage_tex_id: _stages)
    {
        Texture* stage_tex = stage_tex_id.first;

        if (processed.find(stage_tex) != processed.end())
            continue;

        processed.insert(stage_tex);
        index += 1;
        std::string stage_name(stage_tex->get_name());

        const int xoffs = index % entries_per_row;
        const int yoffs = index / entries_per_row;

        NodePath node = _content_node.attach_new_node("Preview");
        node.set_sz(-1);
        node.set_pos(10 + xoffs * (entry_width - 14), 1, yoffs * (entry_height - 14 + 10));

        LVecBase3f rgb(0.2f);
        if (stage_tex_id.second == EntryID::TEXTURE)
            rgb = LVecBase3f(0.2f, 0.4f, 0.6f);

        const std::string internal_name("render_pipeline_internal:");
        auto pos = stage_name.find(internal_name);
        if (pos != std::string::npos)
            stage_name.replace(pos, internal_name.size(), "");

        const std::regex colon_re(":");
        std::vector<std::string> parts(std::sregex_token_iterator(stage_name.begin(), stage_name.end(), colon_re, -1), std::sregex_token_iterator());
        stage_name = parts.back();

        auto df_options = std::make_shared<rppanda::DirectFrame::Options>();
        df_options->frame_size = LVecBase4f(7, entry_width - 17, -7, -entry_height + 17);
        df_options->frame_color = LColorf(rgb, 1.0f);
        df_options->pos = LVecBase3(0, 0, 0);
        rppanda::DirectFrame df(node, df_options);

        auto frame_hover_options = std::make_shared<rppanda::DirectFrame::Options>();
        frame_hover_options->frame_size = LVecBase4(0, entry_width - 10, 0, -entry_height + 10);
        frame_hover_options->frame_color = LColor(0);
        frame_hover_options->pos = LVecBase3(0, 0, 0);
        frame_hover_options->state = rppanda::NORMAL;
        PT(rppanda::DirectFrame) frame_hover = new rppanda::DirectFrame(node, frame_hover_options);
        frame_hovers_.push_back(frame_hover);
        frame_hover->bind(rppanda::ENTER, on_texture_hovered, frame_hover.p());
        frame_hover->bind(rppanda::EXIT, on_texture_blurred, frame_hover.p());
        
        frame_click_data.push_back(std::make_shared<FrameClickDataType>(FrameClickDataType{this, stage_tex}));
        frame_hover->bind(rppanda::B1PRESS, on_texture_clicked, frame_click_data.back().get());

        Text::Parameters params;
        params.text = stage_name;
        params.parent = node;
        params.x = 15;
        params.y = 29;
        params.size = 12;
        params.align = "left";
        params.color = LVecBase3f(0.8f);
        Text stage_text(params);

        // Scale image so it always fits
        int w = stage_tex->get_x_size();
        int h = stage_tex->get_y_size();
        const int padd_x = 24;
        const int padd_y = 57;
        const float scale_x = (entry_width - padd_x) / float((std::max)(1, w));
        const float scale_y = (entry_height - padd_y) / float((std::max)(1, h));
        float scale_factor = (std::min)(scale_x, scale_y);

        if (stage_tex->get_texture_type() == Texture::TextureType::TT_buffer_texture)
        {
            scale_factor = 1.0f;
            w = entry_width - padd_x;
            h = entry_height - padd_y;
        }

        Sprite preview(stage_tex, scale_factor * w, scale_factor * h, node, 7, 40, false, true, false);

        auto preview_node = preview.get_node();
        preview_node->set_shader_input("mipmap", 0);
        preview_node->set_shader_input("slice", 0);
        preview_node->set_shader_input("brightness", 1);
        preview_node->set_shader_input("tonemap", false);
        preview_node->set_shader(DisplayShaderBuilder::build(stage_tex, scale_factor * w, scale_factor * h));
    }

    int num_rows = (index + entries_per_row) / entries_per_row;

    set_scroll_height(50 + (entry_height - 14 + 10) * num_rows);
}

}
