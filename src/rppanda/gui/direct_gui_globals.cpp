#include <render_pipeline/rppanda/gui/direct_gui_globals.h>

#include <pgButton.h>
#include <pgSliderBar.h>
#include <mouseButton.h>
#include <textNode.h>

namespace rppanda {

const std::unordered_map<std::string, PGFrameStyle::Type> FrameStyleDict = {
	{ "flat", FLAT },
	{ "raised", RAISED },
	{ "sunken", SUNKEN },
	{ "groove", GROOVE },
	{ "ridge", RIDGE },
	{ "texture_border", TEXTUREBORDER },
};

const std::string ENTER = PGButton::get_enter_prefix();
const std::string EXIT = PGButton::get_exit_prefix();
const std::string WITHIN = PGButton::get_within_prefix();
const std::string WITHOUT = PGButton::get_without_prefix();

const std::string B1CLICK = PGButton::get_click_prefix() + MouseButton::one().get_name() + '-';
const std::string B2CLICK = PGButton::get_click_prefix() + MouseButton::two().get_name() + '-';
const std::string B3CLICK = PGButton::get_click_prefix() + MouseButton::three().get_name() + '-';
const std::string B1PRESS = PGButton::get_press_prefix() + MouseButton::one().get_name() + '-';
const std::string B2PRESS = PGButton::get_press_prefix() + MouseButton::two().get_name() + "-";
const std::string B3PRESS = PGButton::get_press_prefix() + MouseButton::three().get_name() + "-";
const std::string B1RELEASE = PGButton::get_release_prefix() + MouseButton::one().get_name() + "-";
const std::string B2RELEASE = PGButton::get_release_prefix() + MouseButton::two().get_name() + "-";
const std::string B3RELEASE = PGButton::get_release_prefix() + MouseButton::three().get_name() + "-";

const std::string ADJUST = PGSliderBar::get_adjust_prefix();

TextFont* get_default_font(void)
{
	return TextNode::get_default_font();
}

}
