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
    register_type(type_handle_, "OnscreenImage", DirectObject::get_class_type());
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
