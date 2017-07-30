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
 * This is C++ porting codes of direct/src/gui/OnscreeImage.py
 */

#pragma once

#include <nodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/util/image_input.hpp>

namespace rppanda {

class RENDER_PIPELINE_DECL OnscreenImage: public DirectObject, public NodePath
{
public:
    OnscreenImage(NodePath parent={}, int sort=0);
    OnscreenImage(const std::shared_ptr<ImageInput>& image, NodePath parent={}, int sort=0);

    void set_image(const std::shared_ptr<ImageInput>& image, NodePath parent={}, const TransformState* transform=nullptr, int sort=0);

    void destroy(void);

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************
inline OnscreenImage::OnscreenImage(NodePath parent, int sort)
{
    if (parent.is_empty())
        parent = ShowBase::get_global_ptr()->get_aspect_2d();
    set_image(nullptr, parent, nullptr, sort);
}

inline OnscreenImage::OnscreenImage(const std::shared_ptr<ImageInput>& image, NodePath parent, int sort)
{
    if (parent.is_empty())
        parent = ShowBase::get_global_ptr()->get_aspect_2d();
    set_image(image, parent, nullptr, sort);
}

inline void OnscreenImage::destroy(void)
{
    remove_node();
}

inline TypeHandle OnscreenImage::get_class_type(void)
{
    return type_handle_;
}

inline void OnscreenImage::init_type(void)
{
    DirectObject::init_type();
    register_type(type_handle_, "rppanda::OnscreenImage", DirectObject::get_class_type());
}

inline TypeHandle OnscreenImage::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle OnscreenImage::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
