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

namespace rpcore {

/**
 * Interface for the Panda3D TextNode.
 */
class TextNode : public RPObject
{
public:
    struct RENDER_PIPELINE_DECL Default
    {
        static const float pixel_size;
        static const LVecBase3 color;
        static const std::string align;
        static const std::string font;
    };

public:
    /**
     * Constructs a new text node, forwaring the parameters to the internal
     * panda3d implementation
     */
    TextNode(
        NodePath parent = {},
        float pixel_size = Default::pixel_size,
        const LVecBase2& pos = LVecBase2(0),
        const LVecBase3& color = Default::color,
        const std::string& align = Default::align,
        const std::string& font = Default::font,
        const std::string& text = {});
    ~TextNode();

    /** Returns the node path of the text. */
    NodePath get_np() const;

    /** Returns the current text. */
    std::string get_text() const;

    /** Sets the current text. */
    void set_text(const std::string& text);

    /** Returns the current text color. */
    LColor get_color() const;

    /** Sets the current text color. */
    void set_color(const LColor& color);

    /** Sets the text size in pixels. */
    void set_pixel_size(const LVecBase3& size);
    /** @overload set_pixel_size(const LVecBase3&) */
    void set_pixel_size(PN_stdfloat sx, PN_stdfloat sy, PN_stdfloat sz);
    /** @overload set_pixel_size(const LVecBase3&) */
    void set_pixel_size(PN_stdfloat size);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
