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
 * This is C++ porting codes of direct/src/gui/DirectCheckBox.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_button.hpp>

namespace rppanda {

class ImageInput;

class RENDER_PIPELINE_DECL DirectCheckBox: public DirectButton
{
public:
    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options: public DirectButton::Options
    {
        Options(void);

        std::shared_ptr<ImageInput> checked_image;
        std::shared_ptr<ImageInput> unchecked_image;
        bool is_checked = false;
    };

public:
    DirectCheckBox(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    static void command_func(const Event* ev, void* user_data);

    bool is_checked(void) const;
    void set_checked(bool check);

    std::shared_ptr<ImageInput> get_checked_image(void) const;
    std::shared_ptr<ImageInput> get_unchecked_image(void) const;

protected:
    DirectCheckBox(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

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
inline bool DirectCheckBox::is_checked(void) const
{
    return std::dynamic_pointer_cast<Options>(_options)->is_checked;
}

inline void DirectCheckBox::set_checked(bool check)
{
    std::dynamic_pointer_cast<Options>(_options)->is_checked = check;
}

inline std::shared_ptr<ImageInput> DirectCheckBox::get_checked_image(void) const
{
    return std::dynamic_pointer_cast<Options>(_options)->checked_image;
}

inline std::shared_ptr<ImageInput> DirectCheckBox::get_unchecked_image(void) const
{
    return std::dynamic_pointer_cast<Options>(_options)->unchecked_image;
}

inline TypeHandle DirectCheckBox::get_class_type(void)
{
    return type_handle_;
}

inline void DirectCheckBox::init_type(void)
{
    DirectButton::init_type();
    register_type(type_handle_, "rppanda::DirectCheckBox", DirectButton::get_class_type());
}

inline TypeHandle DirectCheckBox::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle DirectCheckBox::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
