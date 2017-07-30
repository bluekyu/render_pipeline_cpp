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

#include <render_pipeline/rpcore/gui/checkbox.hpp>

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

}
