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
class RENDER_PIPELINE_DECL DirectSlider: public DirectFrame
{
public:
    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options: public DirectFrame::Options
    {
        Options(void);

        LVecBase2 range = LVecBase2(0, 1);
        float value = 0;
        float scroll_size = 0.01f;
        float page_size = 0.1f;
        std::string orientation;

        // Function to be called repeatedly as slider is moved
        std::function<void(void*)> command;
        void* extra_args = nullptr;

        std::shared_ptr<DirectButton::Options> thumb_options;
    };

public:
    DirectSlider(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    PGSliderBar* get_gui_item(void) const;

    void set_range(LVecBase2 range);
    void set_value(float value);

    float get_value(void) const;
    float get_ratio(void) const;

    void set_scroll_size(float scroll_size);
    void set_page_size(float page_size);
    void set_orientation(const std::string& orientation);

    static void command_func(const Event* ev, void* user_data);

protected:
    DirectSlider(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

    PT(DirectButton) thumb_;

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle DirectSlider::get_class_type(void)
{
    return type_handle_;
}

inline void DirectSlider::init_type(void)
{
    DirectFrame::init_type();
    register_type(type_handle_, "rppanda::DirectSlider", DirectFrame::get_class_type());
}

inline TypeHandle DirectSlider::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle DirectSlider::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
