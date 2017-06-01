#include "rpcore/gui/pipe_viewer.h"

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.h>

#include <boost/algorithm/string.hpp>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/stage_manager.hpp>
#include <render_pipeline/rpcore/render_stage.hpp>
#include <render_pipeline/rpcore/gui/sprite.h>
#include <render_pipeline/rpcore/util/generic.h>

#include <render_pipeline/rppanda/gui/direct_scrolled_frame.h>

#include "rpcore/util/display_shader_builder.h"

namespace rpcore {

PipeViewer::PipeViewer(RenderPipeline* pipeline, NodePath parent): DraggableWindow(1300, 900, "Pipeline Visualizer", parent), _pipeline(pipeline)
{
	create_components();
	hide();
}

void PipeViewer::toggle(void)
{
	if (_visible)
	{
		Globals::base->remove_task("UpdatePipeViewer");
		hide();
	}
	else
	{
		Globals::base->add_task(update_task, this, "RP_GUI_UpdatePipeViewer");
		if (!_created)
			populate_content();
		show();
	}
}

AsyncTask::DoneStatus PipeViewer::update_task(GenericAsyncTask* task, void* user_data)
{
	PipeViewer* pv = reinterpret_cast<PipeViewer*>(user_data);
	float scroll_value = pv->_content_frame->get_horizontal_scroll()->get_value();
	scroll_value *= 2.45f;
	pv->_pipe_descriptions.set_x(scroll_value * 2759.0f);
	return AsyncTask::DS_cont;
}

void PipeViewer::create_components(void)
{
	DraggableWindow::create_components();

	auto content_frame_options = std::make_shared<rppanda::DirectScrolledFrame::Options>();
	content_frame_options->frame_size = LVecBase4f(0, _width - 40, 0, _height - 80);
	content_frame_options->canvas_size = LVecBase4f(0, _scroll_width, 0, _scroll_height);
	content_frame_options->auto_hide_scroll_bars = false;
	content_frame_options->scroll_bar_width = 20.0f;
	content_frame_options->frame_color = LColorf(0);
	content_frame_options->vertical_scroll_options->relief = PGFrameStyle::Type(0);
	content_frame_options->horizontal_scroll_options->relief = PGFrameStyle::Type(0);
	content_frame_options->pos = LVecBase3f(20, 1, -_height + 20);
	_content_frame = new rppanda::DirectScrolledFrame(_node, content_frame_options);

	_content_node = _content_frame->get_canvas().attach_new_node("PipeComponents");
	_content_node.set_scale(1, 1, -1);
	_content_node.set_z(_scroll_height);
}

void PipeViewer::populate_content(void)
{
	_created = true;
	_pipe_node = _content_node.attach_new_node("pipes");
	_pipe_node.set_scale(1, 1, -1);
	_stage_node = _content_node.attach_new_node("stages");

	std::vector<std::string> current_pipes;
	const int pipe_pixel_size = 3;
	const int pipe_height = 100;

	// Generate stages
	const auto& stages = _pipeline->get_stage_mgr()->get_stages();
	for (size_t offs=0, offs_end=stages.size(); offs < offs_end; ++offs)
	{
		const std::shared_ptr<RenderStage>& stage = stages[offs];

		NodePath node = _content_node.attach_new_node("stage");
		node.set_pos(220 + offs * 200.0, 0, 20);
		node.set_scale(1, 1, -1);

		{
			auto df_options = std::make_shared<rppanda::DirectFrame::Options>();
			df_options->frame_size = LVecBase4(10, 150, 0, -3600);
			df_options->frame_color = LColor(0.2, 0.2, 0.2, 1);
			rppanda::DirectFrame df(node, df_options);
		}

		std::string stage_name(stage->get_debug_name());
		const std::string key("Stage");
		stage_name.replace(stage_name.find(key), key.length(), "");

		{
			Text::Parameters params;
			params.text = stage_name;
			params.parent = node;
			params.x = 20;
			params.y = 25;
			params.size = 15;
			Text stage_text(params);
		}

		for (const auto& key_val: stage->get_produced_pipes())
		{
			const std::string output_pipe(key_val.get_name());
			const auto& pipe_tex = key_val;

			long long pipe_idx = 0;
			const LVecBase3f& rgb = rgb_from_string(output_pipe);
			auto pipe_iter = std::find(current_pipes.begin(), current_pipes.end(), output_pipe);
			if (pipe_iter != current_pipes.end())
			{
				pipe_idx = std::distance(current_pipes.begin(), pipe_iter);
			}
			else
			{
				current_pipes.push_back(output_pipe);
				pipe_idx = current_pipes.size() - 1;

				auto df_options = std::make_shared<rppanda::DirectFrame::Options>();
				df_options->frame_size = LVecBase4(0, 8000, pipe_pixel_size / 2.0f, -pipe_pixel_size / 2.0f);
				df_options->frame_color = LColor(rgb, 1);
				df_options->pos = LVecBase3(10, 1, -95 - pipe_idx * pipe_height);
				rppanda::DirectFrame output_pipe_df(node, df_options);
			}

			const float w = 160;
			const float h =  Globals::native_resolution.get_y() / float(Globals::native_resolution.get_x()) * w;

			auto df_options = std::make_shared<rppanda::DirectFrame::Options>();
			df_options->frame_size = LVecBase4(-pipe_pixel_size, w + pipe_pixel_size,
				h / 2.0f + pipe_pixel_size,
				-h / 2.0f - pipe_pixel_size);
			df_options->frame_color = LColor(rgb, 1);
			df_options->pos = LVecBase3(0, 1, -95 - pipe_idx * pipe_height);
			rppanda::DirectFrame pipe_df(node, df_options);

			std::string icon_file = "";
            Texture* pipe_texture = nullptr;
            if (pipe_tex.is_type(StageData::Type::SHADER_INPUT))
                pipe_texture = pipe_tex.get_shader_input().get_texture();

			if (pipe_tex.is_type(StageData::Type::SIMPLE_INPUT_BLOCK) || pipe_tex.is_type(StageData::Type::GROUP_INPUT_BLOCK))
			{
				icon_file = "/$$rp/data/gui/icon_ubo.png";
			}
			else if (pipe_texture->get_z_size() > 1)
			{
				icon_file = "/$$rp/data/gui/icon_texture.png";
			}
			else if (pipe_texture->get_texture_type() == Texture::TextureType::TT_buffer_texture)
			{
				icon_file = "/$$rp/data/gui/icon_buffer_texture.png";
			}
			else
			{
				Sprite preview(pipe_texture, w, h, node, 0, 50 + pipe_idx * pipe_height, false, true, false);
				NodePath preview_np = preview.get_node();

				PT(Shader) preview_shader = DisplayShaderBuilder::build(pipe_texture, int(w), int(h));
				preview_np.set_shader(preview_shader);

				preview_np.set_shader_input("mipmap", 0);
				preview_np.set_shader_input("slice", 0);
				preview_np.set_shader_input("brightness", 1);
				preview_np.set_shader_input("tonemap", false);
			}

			if (!icon_file.empty())
			{
				Sprite(icon_file, 48, 48, node, 55, 65 + pipe_idx * pipe_height, true, false);

				std::string tex_desc;
                if (pipe_tex.is_type(StageData::Type::SIMPLE_INPUT_BLOCK) || pipe_tex.is_type(StageData::Type::GROUP_INPUT_BLOCK))
				{
					tex_desc = "UBO";
				}
				else
				{
					tex_desc = pipe_texture->format_texture_type(pipe_texture->get_texture_type());
					tex_desc += std::string(" - ") + boost::to_upper_copy(pipe_texture->format_format(pipe_texture->get_format()));
				}

				{
					Text::Parameters params;
					params.text = tex_desc;
					params.parent = node;
					params.x = 55 + 48 / 2;
					params.y = 130 + pipe_idx * pipe_height;
					params.size = 12;
					params.align = "center";
					params.color = LVecBase3f(0.2f);
					Text tex_desc_text(params);
				}
			}
		}

		for (const auto& input_pipe: stage->get_required_pipes())
		{
			auto pipe_iter = std::find(current_pipes.begin(), current_pipes.end(), input_pipe);
			if (pipe_iter == current_pipes.end())
			{
				warn(std::string("Pipe not found: ") + input_pipe);
				continue;
			}
			long long idx = std::distance(current_pipes.begin(), pipe_iter);
			const LVecBase3& rgb = rgb_from_string(input_pipe);

			auto df_options = std::make_shared<rppanda::DirectFrame::Options>();
			df_options->frame_size = LVecBase4(0, 10, 40, -40);
			df_options->frame_color = LColor(rgb, 1);
			df_options->pos = LVecBase3(5, 1, -95 - idx * pipe_height);
			rppanda::DirectFrame input_pipe_df(node, df_options);
		}
	}


	_pipe_descriptions = _content_node.attach_new_node("PipeDescriptions");
	_pipe_descriptions.set_scale(1, 1, -1);

	{
		auto df_options = std::make_shared<rppanda::DirectFrame::Options>();
		df_options->frame_size = LVecBase4(0, 190, 0, -5000);
		df_options->frame_color = LColor(0.1, 0.1, 0.1, 1.0);
		rppanda::DirectFrame desc_df(_pipe_descriptions, df_options);
	}

	// Generate the pipe descriptions
	for (size_t idx=0, idx_end=current_pipes.size(); idx < idx_end; ++idx)
	{
		const LVecBase3& rgb = rgb_from_string(current_pipes[idx]);

		auto df_options = std::make_shared<rppanda::DirectFrame::Options>();
		df_options->frame_size = LVecBase4(0, 180, -95, -135);
		df_options->frame_color = LColor(rgb, 1.0);
		df_options->pos = LVecBase3(0, 1, -(long long)(idx) * pipe_height);
		rppanda::DirectFrame pipe_desc_df(_pipe_descriptions, df_options);

		Text::Parameters text_params;
		text_params.text = current_pipes[idx];
		text_params.parent = _pipe_descriptions;
		text_params.x = 42;
		text_params.y = 121 + idx * pipe_height;
		text_params.size = 15;
		text_params.align = "left";
		text_params.color = LVecBase3f(0.1);
		Text pipe_text(text_params);

		Sprite("/$$rp/data/gui/icon_pipe.png", _pipe_descriptions, 9, 103 + idx * pipe_height, true, false);
	}
}

}
