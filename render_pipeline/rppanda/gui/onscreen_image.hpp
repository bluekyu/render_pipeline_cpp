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
 * This is C++ porting codes of direct/src/gui/OnscreeImage.py
 */

#pragma once

#include <nodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/util/image_input.hpp>

namespace rppanda {

class RENDER_PIPELINE_DECL OnscreenImage : public DirectObject, public NodePath
{
public:
    OnscreenImage(NodePath parent={}, int sort=0);
    OnscreenImage(const std::shared_ptr<ImageInput>& image, NodePath parent={}, int sort=0);

    ALLOC_DELETED_CHAIN(OnscreenImage);

    void set_image(const std::shared_ptr<ImageInput>& image, NodePath parent={}, const TransformState* transform=nullptr, int sort=0);

    void destroy();

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const override;
    virtual TypeHandle force_init_type() override;

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

inline void OnscreenImage::destroy()
{
    remove_node();
}

inline TypeHandle OnscreenImage::get_class_type()
{
    return type_handle_;
}

inline void OnscreenImage::init_type()
{
    DirectObject::init_type();
    register_type(type_handle_, "rppanda::OnscreenImage", DirectObject::get_class_type());
}

inline TypeHandle OnscreenImage::get_type() const
{
    return get_class_type();
}

inline TypeHandle OnscreenImage::force_init_type()
{
    init_type();
    return get_class_type();
}

}
