/**
 * This is C++ porting codes of direct/src/gui/DirectFrame.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_gui_base.hpp>
#include <render_pipeline/rppanda/util/image_input.hpp>

namespace rppanda {

class RENDER_PIPELINE_DECL DirectFrame: public DirectGuiWidget
{
public:
    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options: public DirectGuiWidget::Options
    {
        Options(void);

        std::vector<std::string> text;
        std::vector<std::shared_ptr<ImageInput>> image;

        bool text_may_change = true;
    };

public:
    DirectFrame(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    void set_text(const std::string& text);
    void set_text(const std::vector<std::string>& text_list);

    void set_image(const std::shared_ptr<ImageInput>& image);
    void set_image(const std::vector<std::shared_ptr<ImageInput>>& images);

protected:
    DirectFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

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
inline void DirectFrame::set_image(const std::shared_ptr<ImageInput>& image)
{
    set_image(std::vector<std::shared_ptr<ImageInput>>({image}));
}

inline TypeHandle DirectFrame::get_class_type(void)
{
    return type_handle_;
}

inline void DirectFrame::init_type(void)
{
    DirectGuiWidget::init_type();
    register_type(type_handle_, "rppanda::DirectFrame", DirectGuiWidget::get_class_type());
}

inline TypeHandle DirectFrame::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle DirectFrame::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
