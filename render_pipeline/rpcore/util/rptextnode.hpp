/**
 * Render Pipeline C++
 *
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

#include <render_pipeline/rpcore/config.hpp>

class TextNode;

namespace rpcore {

/**
 * Decorator for the Panda3D TextNode.
 */
class RENDER_PIPELINE_DECL RPTextNode
{
public:
    struct RENDER_PIPELINE_DECL Default
    {
        static const float pixel_size;
        static const LColor color;
        static const std::string align;
        static const Filename font;
    };

public:
    /**
     * Constructs a new text node, forwaring the parameters to the internal
     * panda3d implementation
     */
    RPTextNode(const std::string& node_name="", NodePath parent={}, float pixel_size=Default::pixel_size,
        const LVecBase3& pos=LVecBase3(0), const LColor& color=Default::color,
        const std::string& align=Default::align, const Filename& font=Default::font,
        const std::string& text="");

    /** Use with existing text node */
    RPTextNode(NodePath np);

    TextNode& operator*() const;
    TextNode* operator->() const;
    explicit operator bool() const;
    bool operator!() const;

    /** Returns the node path of the text. */
    NodePath get_np() const;

    TextNode* get_text_node() const;

    /** Returns the current text. */
    std::string get_text() const;

    /** Sets the current text. */
    void set_text(const std::string& text);

    /** Returns the current text color. */
    LColor get_text_color() const;

    /** Sets the current text color. */
    void set_text_color(const LColor& color);

    /** Sets the text size in pixels. */
    void set_pixel_size(const LVecBase3& size);

    /** @overload set_pixel_size(const LVecBase3&) */
    void set_pixel_size(PN_stdfloat sx, PN_stdfloat sy, PN_stdfloat sz);

    /** @overload set_pixel_size(const LVecBase3&) */
    void set_pixel_size(PN_stdfloat size);

private:
    NodePath nodepath_;
    TextNode* node_;
};

// ************************************************************************************************

inline TextNode& RPTextNode::operator*() const
{
    return *node_;
}

inline TextNode* RPTextNode::operator->() const
{
    return node_;
}

inline RPTextNode::operator bool() const
{
    return node_ != nullptr;
}

inline bool RPTextNode::operator!() const
{
    return node_ == nullptr;
}

inline NodePath RPTextNode::get_np() const
{
    return nodepath_;
}

inline TextNode* RPTextNode::get_text_node() const
{
    return node_;
}

// ************************************************************************************************

inline bool operator==(const RPTextNode& a, const RPTextNode& b)
{
    return a.get_text_node() == b.get_text_node();
}

inline bool operator!=(const RPTextNode& a, const RPTextNode& b)
{
    return a.get_text_node() != b.get_text_node();
}

inline bool operator==(const RPTextNode& m, std::nullptr_t)
{
    return m.get_text_node() == nullptr;
}

inline bool operator==(std::nullptr_t, const RPTextNode& m)
{
    return m.get_text_node() == nullptr;
}

inline bool operator!=(const RPTextNode& m, std::nullptr_t)
{
    return m.get_text_node() != nullptr;
}

inline bool operator!=(std::nullptr_t, const RPTextNode& m)
{
    return m.get_text_node() != nullptr;
}

}
