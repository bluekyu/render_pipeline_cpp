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
 *
 * Some codes come from cOnScreenText.h that written by original author 'dri'.
 * (https://github.com/drivird/drunken-octo-robot/blob/master/p3util/cOnscreenText.h)
 */

#pragma once

#include <nodePath.h>
#include <textNode.h>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.hpp>

class TextFont;

namespace rppanda {

class RENDER_PIPELINE_DECL OnscreenText : public NodePath
{
public:
    // These are the styles of text we might commonly see.  They set the
    // overall appearance of the text according to one of a number of
    // pre-canned styles.  You can further customize the appearance of the
    // text by specifying individual parameters as well.
    enum class Style: int
    {
        plain = 1,
        screen_title,
        screen_prompt,
        name_confirm,
        black_on_white
    };

    struct RENDER_PIPELINE_DECL Default
    {
        static const LVecBase2 shadow_offset;
        static const Style style;
    };

public:
    /**
     * OnscreenText constructor
     * Creates a NodePath that allocates a new TextNode which is initialized with `style'.
     * You should use NodePath::reparent_to() to insert the onscreen text into the aspect 2d
     * of the scene graph.
     *
     * @param[in] style    one of the pre-canned style parameters defined at the
     *                     head of this file.  This sets up the default values for
     *                     many parameters.
     */
    OnscreenText(const std::string& text = "", Style style = Default::style,
        const LVecBase2& pos = LVecBase2(0), PN_stdfloat roll = 0, boost::optional<LVecBase2> scale = boost::none,
        boost::optional<LColor> fg = boost::none, boost::optional<LColor> bg = boost::none, boost::optional<LColor> shadow = boost::none,
        const LVecBase2& shadow_offset = Default::shadow_offset, boost::optional<LColor> frame = boost::none,
        boost::optional<TextNode::Alignment> align = boost::none, boost::optional<PN_stdfloat> wordwrap = boost::none,
        boost::optional<int> draw_order = boost::none, bool decal = false, TextFont* font = nullptr,
        NodePath parent = {}, int sort = 0, bool may_change = true,
        boost::optional<TextProperties::Direction> direction = boost::none);

    void cleanup();

    void destroy() { cleanup(); }

    PandaNode* get_text_node() const;

    /**
     * Reimplementation of TextNode::set_card_decal.
     * @param[in] decal   if this is true, the text is decalled onto its
     *                    background card.  Useful when the text will be parented
     *                    into the 3-D scene graph.
     */
    void set_decal(bool decal);
    bool get_decal() const;

    void set_font(TextFont* font);
    TextFont* get_font() const;

    void clear_text();
    void set_text(const std::string& text);
    void append_text(const std::string& text);
    std::string get_text() const;

    /** Position the onscreen text on the x-axis. */
    void set_x(PN_stdfloat x);
    /** Position the onscreen text on the y-axis. */
    void set_y(PN_stdfloat y);
    /** Position the onscreen text in 2d screen space. */
    void set_pos(PN_stdfloat x, PN_stdfloat y);
    void set_pos(const LVecBase2& pos);
    const LVecBase2& get_pos() const;

    void set_roll(PN_stdfloat roll);
    PN_stdfloat get_roll() const;

    void set_scale(PN_stdfloat scale);
    void set_scale(PN_stdfloat scaleX, PN_stdfloat scaleY);

    /**
     * Scale the text in 2d space.  You may specify either a single
     * uniform scale, or two scales, or a tuple of two scales.
     */
    void set_scale(const LVecBase2& scale);

    void update_transform_mat();
    const LVecBase2& get_scale() const;

    void set_wordwrap(PN_stdfloat wordwarp);
    PN_stdfloat get_wordwrap() const;

    void set_fg(const LColor& fg);
    void set_bg(const LColor& bg);
    void set_shadow(const LColor& shadow);
    void set_shadow_offset(const LVecBase2& offset);
    void set_frame(const LColor& frame);

    void set_align(TextNode::Alignment align);

private:
    LVecBase2 scale_;
    LVecBase2 pos_;
    PN_stdfloat roll_;
    PN_stdfloat wordwrap_;
    bool may_change_;
    bool is_clean_;

    PandaNode* text_node_;        ///< This is just for access.
};

// ************************************************************************************************

inline PandaNode* OnscreenText::get_text_node() const
{
    return text_node_;
}

inline void OnscreenText::set_x(PN_stdfloat x)
{
    set_pos(LVecBase2(x, pos_[1]));
}

inline void OnscreenText::set_y(PN_stdfloat y)
{
    set_pos(pos_[0], y);
}

inline void OnscreenText::set_pos(PN_stdfloat x, PN_stdfloat y)
{
    set_pos(LVecBase2(x, y));
}

inline const LVecBase2& OnscreenText::get_pos() const
{
    return pos_;
}

inline PN_stdfloat OnscreenText::get_roll() const
{
    return roll_;
}

inline void OnscreenText::set_scale(PN_stdfloat scale)
{
    set_scale(LVecBase2(scale, scale));
}

inline void OnscreenText::set_scale(PN_stdfloat scaleX, PN_stdfloat scaleY)
{
    set_scale(LVecBase2(scaleX, scaleY));
}

inline PN_stdfloat OnscreenText::get_wordwrap() const
{
    return wordwrap_;
}

inline void OnscreenText::update_transform_mat()
{
    const LMatrix4& mat =
        LMatrix4::scale_mat(scale_.get_x(), 1, scale_.get_y()) *
        LMatrix4::rotate_mat(roll_, LVector3::back()) *
        LMatrix4::translate_mat(pos_.get_x(), 0, pos_.get_y());
    DCAST(TextNode, text_node_)->set_transform(mat);
}

}
