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

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpcore {

class Text;
class Checkbox;

/**
 * This is a checkbox, combined with a label. The arguments are
 * equal to the Checkbox and OnscreenText arguments.
 */
class RENDER_PIPELINE_DECL LabeledCheckbox : public RPObject
{
public:
    struct Default
    {
        static const float text_size;
        static const int expand_width = 100;
    };

public:
    /**
     * Constructs a new checkbox, forwarding most of the elements to the
     * underlying Checkbox and Text.
     */
    LabeledCheckbox(
        NodePath parent={}, float x=0, float y=0,
        const std::function<void(bool, const std::shared_ptr<void>&)>& chb_callback={},
        const std::shared_ptr<void>& chb_args={}, bool chb_checked=false,
        const std::string& text={}, float text_size=Default::text_size,
        bool radio=false, const boost::optional<LVecBase3>& text_color={},
        int expand_width=Default::expand_width, bool enabled=true);
    ~LabeledCheckbox();

    /** Returns a handle to the checkbox. */
    Checkbox* get_checkbox() const;

    /** Returns a handle to the label. */
    Text* get_label() const;

    const LVecBase3& get_text_color() const;

private:
    /** Internal callback when the node gets hovered. */
    static void on_node_enter(const Event* ev, void* user_data);

    /** Internal callback when the node gets no longer hovered. */
    static void on_node_leave(const Event* ev, void* user_data);

    Checkbox* checkbox_;
    Text* text_;
    LVecBase3 text_color_;
};

inline Checkbox* LabeledCheckbox::get_checkbox() const
{
    return checkbox_;
}

inline Text* LabeledCheckbox::get_label() const
{
    return text_;
}

inline const LVecBase3& LabeledCheckbox::get_text_color() const
{
    return text_color_;
}

}
