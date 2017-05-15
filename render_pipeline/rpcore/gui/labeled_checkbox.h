#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/gui/checkbox.h>

namespace rpcore {

class Text;

/**
 * This is a checkbox, combined with a label. The arguments are
 * equal to the Checkbox and OnscreenText arguments.
 */
class RENDER_PIPELINE_DECL LabeledCheckbox: public RPObject
{
public:
	struct Parameters: public Checkbox::Parameters
	{
		std::string text = "";
		float text_size = 16;
		LVecBase3f text_color = LVecBase3f(1);
	};

public:
	LabeledCheckbox(const Parameters& params=Parameters());
	~LabeledCheckbox(void);

	/** Returns a handle to the checkbox. */
	Checkbox* get_checkbox(void) const;

	/** Returns a handle to the label. */
	Text* get_label(void) const;

private:
	/** Internal callback when the node gets hovered. */
	static void on_node_enter(const Event* ev, void* user_data);

	/** Internal callback when the node gets no longer hovered. */
	static void on_node_leave(const Event* ev, void* user_data);

	Checkbox* _checkbox;
	Text* _text;
	LVecBase3f _text_color;
};

inline Checkbox* LabeledCheckbox::get_checkbox(void) const
{
	return _checkbox;
}

inline Text* LabeledCheckbox::get_label(void) const
{
	return _text;
}

}	// namespace rpcore
