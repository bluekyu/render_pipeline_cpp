#include "render_pipeline/rpcore/gui/text.h"
#include "render_pipeline/rpcore/globals.hpp"

#include <map>

#include "textNode.h"
#include "textFont.h"

namespace rpcore {

Text::Text(const Parameters& params): RPObject("Text")
{
	::TextNode::Alignment align_mode = ::TextNode::Alignment::A_left;

	if (params.align == "center")
		align_mode = ::TextNode::Alignment::A_center;
	else if (params.align == "right")
		align_mode = ::TextNode::Alignment::A_right;

	TextFont* font = params.font;
	if (!font)
	{
		font = Globals::font;
		assert(font);
	}

	_initial_pos = LVecBase2f(params.x, -params.y);

	rppanda::OnscreenText::Parameters node_params;
	node_params.text = params.text;
	node_params.parent = params.parent;
	node_params.pos = _initial_pos;
	node_params.scale = params.size;
	node_params.align = align_mode;
	node_params.fg = LColorf(params.color, 1.0f);
	node_params.font = font;
	node_params.may_change = params.may_change;
	_node = rppanda::OnscreenText(node_params);
}

}
