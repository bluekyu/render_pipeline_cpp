#pragma once

#include <render_pipeline/rppanda/gui/direct_frame.h>
#include <render_pipeline/rppanda/gui/direct_scroll_bar.h>

class PGScrollFrame;

namespace rppanda {

class DirectScrolledFrame: public DirectFrame
{
public:
	class Options: public DirectFrame::Options
	{
	public:
		Options(void);
		virtual ~Options(void) = default;

		LVecBase4f canvas_size = LVecBase4f(-1, 1, -1, 1);
		bool manage_scroll_bars = true;
		bool auto_hide_scroll_bars = true;
		float scroll_bar_width = 0.08f;

		std::shared_ptr<DirectScrollBar::Options> vertical_scroll_options;
		std::shared_ptr<DirectScrollBar::Options> horizontal_scroll_options;
	};

public:
	DirectScrolledFrame(NodePath parent=NodePath(), const std::shared_ptr<Options>& options=std::make_shared<Options>());

	PGScrollFrame* get_gui_item(void) const;

	NodePath get_canvas(void);
	const std::shared_ptr<DirectScrollBar>& get_vertical_scroll(void) const;
	const std::shared_ptr<DirectScrollBar>& get_horizontal_scroll(void) const;

	const LVecBase4f& get_canvas_size(void) const;
	bool get_manage_scroll_bars(void) const;
	bool get_auto_hide_scroll_bars(void) const;

	void set_canvas_size(const LVecBase4f& canvas_size);
	void set_manage_scroll_bars(bool manage_scroll_bars);
	void set_auto_hide_scroll_bars(bool auto_hide_scroll_bars);

protected:
	DirectScrolledFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle);

	void initialise_options(const std::shared_ptr<Options>& options);

private:
	const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

	NodePath _canvas;

	std::shared_ptr<DirectScrollBar> _vertical_scroll;
	std::shared_ptr<DirectScrollBar> _horizontal_scroll;

public:
	static const std::type_info& get_class_type(void) { return _type_handle; }
	virtual const std::type_info& get_type(void) const { return get_class_type(); }

private:
	static const std::type_info& _type_handle;
};

inline NodePath DirectScrolledFrame::get_canvas(void)
{
	return _canvas;
}

inline const std::shared_ptr<DirectScrollBar>& DirectScrolledFrame::get_vertical_scroll(void) const
{
	return _vertical_scroll;
}

inline const std::shared_ptr<DirectScrollBar>& DirectScrolledFrame::get_horizontal_scroll(void) const
{
	return _horizontal_scroll;
}

inline const LVecBase4f& DirectScrolledFrame::get_canvas_size(void) const
{
	return std::dynamic_pointer_cast<Options>(_options)->canvas_size;
}

inline bool DirectScrolledFrame::get_manage_scroll_bars(void) const
{
	return std::dynamic_pointer_cast<Options>(_options)->manage_scroll_bars;
}

inline bool DirectScrolledFrame::get_auto_hide_scroll_bars(void) const
{
	return std::dynamic_pointer_cast<Options>(_options)->auto_hide_scroll_bars;
}

}
