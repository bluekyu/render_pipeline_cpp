#pragma once

#include <render_pipeline/rppanda/gui/direct_gui_base.h>
#include <render_pipeline/rppanda/util/image_input.h>

namespace rppanda {

class DirectFrame: public DirectGuiWidget
{
public:
    class Options: public DirectGuiWidget::Options
    {
    public:
        Options(void);
        virtual ~Options(void) = default;

        std::vector<std::string> text;
        std::vector<std::shared_ptr<ImageInput>> image;

        bool text_may_change = true;
    };

public:
    DirectFrame(NodePath parent=NodePath(), const std::shared_ptr<Options>& options=std::make_shared<Options>());

    void set_text(const std::string& text);
    void set_text(const std::vector<std::string>& text_list);

    void set_image(const std::shared_ptr<ImageInput>& image);
    void set_image(const std::vector<std::shared_ptr<ImageInput>>& images);

protected:
    DirectFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

public:
    static const std::type_info& get_class_type(void) { return _type_handle; }
    virtual const std::type_info& get_type(void) const { return get_class_type(); }

private:
    static const std::type_info& _type_handle;
};

// ************************************************************************************************
inline void DirectFrame::set_image(const std::shared_ptr<ImageInput>& image)
{
    set_image(std::vector<std::shared_ptr<ImageInput>>({image}));
}

}
