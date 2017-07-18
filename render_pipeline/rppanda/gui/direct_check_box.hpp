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
    DirectCheckBox(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle);

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

}
