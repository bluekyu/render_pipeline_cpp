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
class DirectSlider: public DirectFrame
{
public:
    struct Options: public DirectFrame::Options
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
    DirectSlider(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

    std::shared_ptr<DirectButton> thumb_;

public:
    static const std::type_info& get_class_type(void) { return type_handle_; }
    virtual const std::type_info& get_type(void) const { return get_class_type(); }

private:
    static const std::type_info& type_handle_;
};

}
