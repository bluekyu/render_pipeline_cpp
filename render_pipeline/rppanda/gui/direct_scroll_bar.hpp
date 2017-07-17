/**
 * This is C++ porting codes of direct/src/gui/DirectScrollBar.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_frame.hpp>
#include <render_pipeline/rppanda/gui/direct_button.hpp>

class PGSliderBar;

namespace rppanda {

class RENDER_PIPELINE_DECL DirectScrollBar: public DirectFrame
{
public:
	struct RENDER_PIPELINE_DECL Options: public DirectFrame::Options
	{
		Options(void);

		LVecBase2 range = LVecBase2(0, 1);
		PN_stdfloat value = 0;
		PN_stdfloat scroll_size = PN_stdfloat(0.01);
		PN_stdfloat page_size = PN_stdfloat(0.1);
		std::string orientation;
		bool manage_buttons = true;
		bool resize_thumb = true;

		std::shared_ptr<DirectButton::Options> thumb_options;
		std::shared_ptr<DirectButton::Options> inc_button_options;
		std::shared_ptr<DirectButton::Options> dec_button_options;
	};

public:
	DirectScrollBar(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

	PGSliderBar* get_gui_item(void) const;

	float get_value(void) const;

	void set_range(const LVecBase2& range);
	void set_value(PN_stdfloat value);
	void set_scroll_size(PN_stdfloat scroll_size);
	void set_page_size(PN_stdfloat page_size);
	void set_orientation(const std::string& orientation);
	void set_manage_buttons(bool manage_buttons);
	void set_resize_thumb(bool resize_thumb);

	static void command_func(const Event* ev, void* user_data);

protected:
	DirectScrollBar(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle);

	void initialise_options(const std::shared_ptr<Options>& options);

private:
	const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

	std::shared_ptr<DirectButton> _thumb;
	std::shared_ptr<DirectButton> _inc_button;
	std::shared_ptr<DirectButton> _dec_button;

public:
	static const std::type_info& get_class_type(void) { return _type_handle; }
	virtual const std::type_info& get_type(void) const { return get_class_type(); }

private:
	static const std::type_info& _type_handle;
};

inline float DirectScrollBar::get_value(void) const
{
	return std::dynamic_pointer_cast<Options>(_options)->value;
}

}
