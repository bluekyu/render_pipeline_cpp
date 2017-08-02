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
        std::function<void(const std::shared_ptr<void>&)> command;
        std::shared_ptr<void> extra_args;

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
