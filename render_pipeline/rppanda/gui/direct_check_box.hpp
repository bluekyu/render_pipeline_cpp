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
        Options();

        std::shared_ptr<ImageInput> checked_image;
        std::shared_ptr<ImageInput> unchecked_image;
        bool is_checked = false;
    };

public:
    DirectCheckBox(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    static void command_func(const Event* ev, void* user_data);

    bool is_checked() const;
    void set_checked(bool check);

    std::shared_ptr<ImageInput> get_checked_image() const;
    std::shared_ptr<ImageInput> get_unchecked_image() const;

protected:
    DirectCheckBox(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************
inline bool DirectCheckBox::is_checked() const
{
    return std::dynamic_pointer_cast<Options>(_options)->is_checked;
}

inline void DirectCheckBox::set_checked(bool check)
{
    std::dynamic_pointer_cast<Options>(_options)->is_checked = check;
}

inline std::shared_ptr<ImageInput> DirectCheckBox::get_checked_image() const
{
    return std::dynamic_pointer_cast<Options>(_options)->checked_image;
}

inline std::shared_ptr<ImageInput> DirectCheckBox::get_unchecked_image() const
{
    return std::dynamic_pointer_cast<Options>(_options)->unchecked_image;
}

inline TypeHandle DirectCheckBox::get_class_type()
{
    return type_handle_;
}

inline void DirectCheckBox::init_type()
{
    DirectButton::init_type();
    register_type(type_handle_, "rppanda::DirectCheckBox", DirectButton::get_class_type());
}

inline TypeHandle DirectCheckBox::get_type() const
{
    return get_class_type();
}

inline TypeHandle DirectCheckBox::force_init_type()
{
    init_type();
    return get_class_type();
}

}
