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
 * This is C++ porting codes of direct/src/gui/DirectSlider.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_frame.hpp>
#include <render_pipeline/rppanda/gui/direct_button.hpp>

class PGSliderBar;

namespace rppanda {

/**
 * DirectSlider -- a widget which represents a slider that the
 * user can pull left and right to represent a continuous value.
 */
class RENDER_PIPELINE_DECL DirectSlider : public DirectFrame
{
public:
    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options : public DirectFrame::Options
    {
        Options();

        LVecBase2 range = LVecBase2(0, 1);
        float value = 0;
        float scroll_size = 0.01f;
        float page_size = 0.1f;
        std::string orientation;

        // Function to be called repeatedly as slider is moved
        std::function<void()> command;
        // bind "extraArgs" to std::function

        std::shared_ptr<DirectButton::Options> thumb_options;
    };

public:
    DirectSlider(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());
    virtual ~DirectSlider();

    ALLOC_DELETED_CHAIN(DirectSlider);

    PGSliderBar* get_gui_item() const;

    void set_range(LVecBase2 range);
    void set_value(float value);

    float get_value() const;
    float get_ratio() const;

    void set_scroll_size(float scroll_size);
    void set_page_size(float page_size);
    void set_orientation(const std::string& orientation);

    void command_func();

protected:
    DirectSlider(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

    PT(DirectButton) thumb_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle DirectSlider::get_class_type()
{
    return type_handle_;
}

inline void DirectSlider::init_type()
{
    DirectFrame::init_type();
    register_type(type_handle_, "rppanda::DirectSlider", DirectFrame::get_class_type());
}

inline TypeHandle DirectSlider::get_type() const
{
    return get_class_type();
}

inline TypeHandle DirectSlider::force_init_type()
{
    init_type();
    return get_class_type();
}

}
