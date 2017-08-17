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

#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>

class TextFont;

namespace rpcore {

/** Simple wrapper around OnscreenText, providing a simpler interface. */
class Text: public RPObject
{
public:
    struct Parameters
    {
        std::string text = "";
        NodePath parent = NodePath();
        float x = 0.0f;
        float y = 0.0f;
        float size = 10.0f;
        std::string align = "left";
        LVecBase3 color = LVecBase3(1);
        bool may_change = false;
        TextFont* font = nullptr;
    };

public:
    /**
     * Constructs a new text. The parameters are almost equal to the
     * parameters of OnscreenText.
     */
    Text(const Parameters& params=Parameters());

    rppanda::OnscreenText get_node() const;

    /**
     * Changes the text, remember to pass may_change to the constructor,
     * otherwise this method does not work.
     */
    void set_text(const std::string& text);

    const LVecBase2f& get_initial_pos() const;

private:
    LVecBase2f _initial_pos;
    rppanda::OnscreenText _node;
};

inline rppanda::OnscreenText Text::get_node() const
{
    return _node;
}

inline void Text::set_text(const std::string& text)
{
    _node.set_text(text);
}

inline const LVecBase2f& Text::get_initial_pos() const
{
    return _initial_pos;
}

}
