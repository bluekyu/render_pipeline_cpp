#pragma once

#include <render_pipeline/rpcore/gui/draggable_window.h>

namespace rppanda {
class DirectScrolledFrame;
class DirectFrame;
}

namespace rpcore {

class RenderPipeline;
class TexturePreview;
class RenderStage;
class LabeledCheckbox;

/** This class provides a view into the buffers to inspect them . */
class BufferViewer: public DraggableWindow
{
	enum class EntryID: int
	{
		TEXTURE = 0,
		RENDER_TARGET,
	};

public:
	using EntryType = std::pair<void*, EntryID>;

	BufferViewer(RenderPipeline* pipeline, NodePath parent);

	void toggle(void);

	std::vector<EntryType> get_entries(void) const;

	/** Returns the amount of attached stages, and also the memory consumed in MiB in a tuple. */
	std::pair<size_t, int> get_stage_information(void) const;

private:
	/** Creates the window components. */
	virtual void create_components(void);

	/** Sets whether images and textures will be shown. */
	void set_show_images(bool arg, void* extra_args);

	/** Sets the maximum scroll height in the content frame */
	void set_scroll_height(int height);

	/** Removes all components of the buffer viewer. */
	void remove_components(void);

	/** Collects all entries, extracts their images and re-renders the window. */
	void perform_update(void);

	/** Internal method when a texture is hovered. */
	static void on_texture_hovered(const Event* func, void* data);

	/** Internal method when a texture is blurred. */
	static void on_texture_blurred(const Event* func, void* data);

	/** Internal method when a texture is clicked. */
	static void on_texture_clicked(const Event* func, void* data);

	/** Renders the stages to the window. */
	void render_stages(void);

	RenderPipeline* _pipeline;
	int _scroll_height = 3000;
	bool _display_images = false;
	std::shared_ptr<rppanda::DirectScrolledFrame> _content_frame;
	NodePath _content_node;
	std::shared_ptr<LabeledCheckbox> _chb_show_images;

    std::vector<std::pair<Texture*, EntryID>> _stages;
	std::shared_ptr<TexturePreview> _tex_preview;

	std::vector<std::shared_ptr<rppanda::DirectFrame>> frame_hovers_;

	struct FrameClickDataType
	{
		BufferViewer* self;
		Texture* tex;
	};
	std::vector<std::shared_ptr<FrameClickDataType>> frame_click_data;
};

}
