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

/**
 * This is C++ porting codes of direct/src/gui/DirectButton.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_frame.hpp>

class PGButton;

namespace rppanda {

/**
 * DirectButton(parent) - Create a DirectGuiWidget which responds
 * to mouse clicks and execute a callback function if defined.
 */
class RENDER_PIPELINE_DECL DirectButton: public DirectFrame
{
public:
    /** Parameter Container. */
    struct RENDER_PIPELINE_DECL Options: public DirectFrame::Options
    {
        Options(void);

        /// Command to be called on button click.
        std::function<void(void*)> command;
        void* extra_args = nullptr;

        /// Which mouse buttons can be used to click the button.
        std::vector<int> command_buttons;

        /**
         * Can only be specified at time of widget contruction
         * Do the text/graphics appear to move when the button is clicked
         */
        bool press_effect = true;

    protected:
        friend class DirectButton;

        void (*command_func)(const Event*, void*) = nullptr;
    };

public:
    DirectButton(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    PGButton* get_gui_item(void) const;

    void set_command_buttons(void);

    static void command_func(const Event* ev, void* user_data);

    void set_click_sound(void);

    void set_rollover_sound(void);

protected:
    DirectButton(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle DirectButton::get_class_type(void)
{
    return type_handle_;
}

inline void DirectButton::init_type(void)
{
    DirectGuiWidget::init_type();
    register_type(type_handle_, "rppanda::DirectButton", DirectGuiWidget::get_class_type());
}

inline TypeHandle DirectButton::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle DirectButton::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
