/**
 * Copyright (c) 2008, Carnegie Mellon University.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Carnegie Mellon University nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * (This is the Modified BSD License.  See also
 * http://www.opensource.org/licenses/bsd-license.php )
 */

/**
 * This is C++ porting codes of direct/src/gui/OnscreeText.py
 */

#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/gui/direct_gui_globals.hpp>

namespace rppanda {

static const LVecBase2f SHADOW_OFFSET(0.04f);
static const float MARGIN = 0.1f;

OnscreenText::OnscreenText(const Parameters& params)
{
    NodePath parent = params.parent;
    if (parent.is_empty())
        parent = rppanda::ShowBase::get_global_ptr()->get_aspect_2d();

    // make a text node
    PT(TextNode) text_node = new TextNode("");
    _text_node = text_node;

    // Choose the default parameters according to the selected style.
    LColorf fg;
    LColorf bg;
    LColorf shadow;
    LColorf frame;
    boost::optional<TextNode::Alignment> align = params.align;

    switch (params.style)
    {
    case Style::plain:
        _scale = params.scale ? params.scale.get() : 0.07f;
        fg = params.fg ? params.fg.get() : LColorf(0, 0, 0, 1);
        bg = params.bg ? params.bg.get() : LColorf(0, 0, 0, 0);
        shadow = params.shadow ? params.shadow.get() : LColorf(0, 0, 0, 0);
        frame = params.frame ? params.frame.get() : LColorf(0, 0, 0, 0);
        if (!align)
            align = TextNode::A_center;
        break;
    case Style::screen_title:
        _scale = params.scale ? params.scale.get() : 0.15f;
        fg = params.fg ? params.fg.get() : LColorf(1.0f, 0.2f, 0.2f, 1.0f);
        bg = params.bg ? params.bg.get() : LColorf(0, 0, 0, 0);
        shadow = params.shadow ? params.shadow.get() : LColorf(0, 0, 0, 1);
        frame = params.frame ? params.frame.get() : LColorf(0, 0, 0, 0);
        if (!align)
            align = TextNode::A_center;
        break;
    case Style::screen_prompt:
        _scale = params.scale ? params.scale.get() : 0.1f;
        fg = params.fg ? params.fg.get() : LColorf(1, 1, 0, 1);
        bg = params.bg ? params.bg.get() : LColorf(0, 0, 0, 0);
        shadow = params.shadow ? params.shadow.get() : LColorf(0, 0, 0, 1);
        frame = params.frame ? params.frame.get() : LColorf(0, 0, 0, 0);
        if (!align)
            align = TextNode::A_center;
        break;
    case Style::name_confirm:
        _scale = params.scale ? params.scale.get() : 0.1f;
        fg = params.fg ? params.fg.get() : LColorf(0, 1, 0, 1);
        bg = params.bg ? params.bg.get() : LColorf(0, 0, 0, 0);
        shadow = params.shadow ? params.shadow.get() : LColorf(0, 0, 0, 0);
        frame = params.frame ? params.frame.get() : LColorf(0, 0, 0, 0);
        if (!align)
            align = TextNode::A_center;
        break;

    case Style::black_on_white:
        _scale = params.scale ? params.scale.get() : 0.1f;
        fg = params.fg ? params.fg.get() : LColorf(0, 0, 0, 1);
        bg = params.bg ? params.bg.get() : LColorf(1, 1, 1, 0);
        shadow = params.shadow ? params.shadow.get() : LColorf(0, 0, 0, 0);
        frame = params.frame ? params.frame.get() : LColorf(0, 0, 0, 0);
        if (!align)
            align = TextNode::A_center;
        break;

    default:
        nout << "DESIGN ERROR: missing a TextStyle case." << endl;
        return;
    }

    // Save some of the parameters for posterity.
    _pos = params.pos;
    _roll = params.roll;

    if (params.decal)
        text_node->set_card_decal(true);

    text_node->set_font(params.font ? params.font : get_default_font());
    text_node->set_text_color(fg);
    text_node->set_align(align.get());

    if (params.wordwrap)
        text_node->set_wordwrap(params.wordwrap.get());

    if (bg[3] != 0)
    {
        // If we have a background color, create a card.
        text_node->set_card_color(bg);
        text_node->set_card_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
    }

    if (shadow[3] != 0)
    {
        // If we have a shadow color, create a shadow.
        // Can't use the *shadow interface because it might be a VBase4.
        // textNode.setShadowColor(*shadow)
        text_node->set_shadow_color(shadow);
        text_node->set_shadow(params.shadow_offset);
    }

    if (frame[3] != 0)
    {
        // If we have a frame color, create a frame.
        text_node->set_frame_color(frame);
        text_node->set_frame_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
    }

    // Create a transform for the text for our scale and position.
    // We'd rather do it here, on the text itself, rather than on
    // our NodePath, so we have one fewer transforms in the scene
    // graph.
    update_transform_mat();

    if (params.draw_order)
    {
        text_node->set_bin("fixed");
        text_node->set_draw_order(params.draw_order.get());
    }

    set_text(params.text);
    if (!params.text.empty())
        _may_change = true;
    else
        _may_change = params.may_change;

    // Ok, now update the node.
    if (!_may_change)
        _text_node = text_node->generate();

    _is_clean = false;

    // Set ourselves up as the NodePath that points to this node.
    NodePath::operator=(std::move(parent.attach_new_node(_text_node, params.sort)));
}

void OnscreenText::cleanup(void)
{
    _text_node = nullptr;
    if (!_is_clean)
    {
        _is_clean = true;
        remove_node();
    }
}

void OnscreenText::set_decal(bool decal)
{
    DCAST(TextNode, _text_node)->set_card_decal(decal);
}

bool OnscreenText::get_decal(void) const
{
    return DCAST(TextNode, _text_node)->get_card_decal();
}

// Reimplementation of TextNode::set_font.
// fontPtr: the font to use for the text.
void OnscreenText::set_font(TextFont* fontPtr)
{
    DCAST(TextNode, _text_node)->set_font(fontPtr);
}

// Reimplementation of TextNode::get_font.
TextFont* OnscreenText::get_font() const
{
    return DCAST(TextNode, _text_node)->get_font();
}

// Reimplementation of TextNode::clear_text.
void OnscreenText::clear_text()
{
    DCAST(TextNode, _text_node)->clear_text();
}

// Reimplementation of TextNode::set_text.
// text: the actual text to display.
void OnscreenText::set_text(const string& text)
{
    DCAST(TextNode, _text_node)->set_text(text);
}

// Reimplementation of TextNode::append_text.
void OnscreenText::append_text(const string& text)
{
    DCAST(TextNode, _text_node)->append_text(text);
}

// Reimplementation of TextNode::get_text.
string OnscreenText::get_text() const
{
    return DCAST(TextNode, _text_node)->get_text();
}

// Position the onscreen text in 2d screen space.
// pos: the x, y position of the text on the screen.
void OnscreenText::set_pos(const LVecBase2f& pos)
{
    _pos = pos;
    update_transform_mat();
}

// Rotate the onscreen text around the screen's normal
void OnscreenText::set_roll(float roll)
{
    _roll = roll;
    update_transform_mat();
}

void OnscreenText::set_scale(const LVecBase2f& scale)
{
    _scale = scale;
    update_transform_mat();
}

// Returns the scale of the text in 2d space.
const LVecBase2f& OnscreenText::get_scale() const
{
    return _scale;
}


// Reimplementation of TextNode::set_wordwrap.
// wordwrap: either the width to wordwrap the text at, or 0
//           to specify no automatic word wrapping.
void OnscreenText::set_wordwrap(float wordwrap)
{
    _wordwrap = wordwrap;
    if (wordwrap != 0)
    {
        DCAST(TextNode, _text_node)->set_wordwrap(wordwrap);
    }
    else
    {
        DCAST(TextNode, _text_node)->clear_wordwrap();
    }
}

// Reimplementation of TextNode::set_text_color.
// fg: the (r, g, b, a) foreground color of the text.
void OnscreenText::set_fg(const LColor& fg)
{
    DCAST(TextNode, _text_node)->set_text_color(fg);
}

// Reimplementation of TextNode::set_card_color
// bg: the (r, g, b, a) background color of the text.  If the
//     fourth value, a, is nonzero, a card is created to place
//     behind the text and set to the given color.
void OnscreenText::set_bg(const LColor& bg)
{
    if (bg[3] != 0)
    {
        // If we have a background color, create a card.
        DCAST(TextNode, _text_node)->set_card_color(bg);
        DCAST(TextNode, _text_node)->set_card_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
    }
    else
    {
        // Otherwise, remove the card.
        DCAST(TextNode, _text_node)->clear_card();
    }
}

// Reimplementation of both TextNode::set_shadow_color and TextNode::set_shadow.
// shadow: the (r, g, b, a) color of the shadow behind the text.
//         If the fourth value, a, is nonzero, a little drop shadow
//         is created and placed behind the text.
void OnscreenText::set_shadow(const LColor& shadow)
{
    if (shadow[3] != 0)
    {
        // If we have a shadow color, create a shadow.
        DCAST(TextNode, _text_node)->set_shadow_color(shadow);
        DCAST(TextNode, _text_node)->set_shadow(SHADOW_OFFSET);
    }
    else
    {
        // Otherwise, remove the shadow.
        DCAST(TextNode, _text_node)->clear_shadow();
    }
}

// Reimplementation of TextNode::set_shadow.
void OnscreenText::set_shadow_offset(const LVecBase2& offset)
{
    DCAST(TextNode, _text_node)->set_shadow(offset);
}

// Reimplementation of TextNode::set_frame_color
// frame: the (r, g, b, a) color of the frame drawn around the
//        text.  If the fourth value, a, is nonzero, a frame is
//        created around the text.
void OnscreenText::set_frame(const LColor& frame)
{
    if (frame[3] != 0)
    {
        // If we have a frame color, create a frame.
        DCAST(TextNode, _text_node)->set_frame_color(frame);
        DCAST(TextNode, _text_node)->set_frame_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
    }
    else
    {
        // Otherwise, remove the frame.
        DCAST(TextNode, _text_node)->clear_frame();
    }
}

// Reimplementation of TextNode::set_align
// align: one of TextNode::A_Left, TextNode::A_right, or TextNode::A_center.
void OnscreenText::set_align(TextNode::Alignment align)
{
    return DCAST(TextNode, _text_node)->set_align(align);
}

}
