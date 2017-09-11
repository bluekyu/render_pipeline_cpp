/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

class TextFont;

namespace rpcore {

/** Simple wrapper around OnscreenText, providing a simpler interface. */
class RENDER_PIPELINE_DECL Text : public RPObject
{
public:
    struct RENDER_PIPELINE_DECL Default
    {
        static const float size;
        static const std::string align;
        static const LVecBase3 color;
    };

public:
    /**
     * Constructs a new text. The parameters are almost equal to the
     * parameters of OnscreenText.
     */
    Text(const std::string& text={}, NodePath parent={}, float x=0, float y=0, float size=Default::size,
        const std::string& align=Default::align, const LVecBase3& color=Default::color,
        bool may_change=false, TextFont* font=nullptr);

    const rppanda::OnscreenText& get_node() const;
    rppanda::OnscreenText& get_node();

    /**
     * Changes the text, remember to pass may_change to the constructor,
     * otherwise this method does not work.
     */
    void set_text(const std::string& text);

    const LVecBase2& get_initial_pos() const;

private:
    LVecBase2 initial_pos_;
    rppanda::OnscreenText node_;
};

inline const rppanda::OnscreenText& Text::get_node() const
{
    return node_;
}

inline rppanda::OnscreenText& Text::get_node()
{
    return node_;
}

inline void Text::set_text(const std::string& text)
{
    node_.set_text(text);
}

inline const LVecBase2& Text::get_initial_pos() const
{
    return initial_pos_;
}

}
