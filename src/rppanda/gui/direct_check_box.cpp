/**
 * This is C++ porting codes of direct/src/gui/DirectCheckBox.py
 */

#include <render_pipeline/rppanda/gui/direct_check_box.hpp>

#include <pgButton.h>

#include <render_pipeline/rppanda/util/image_input.hpp>
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

TypeHandle DirectCheckBox::type_handle_;

DirectCheckBox::Options::Options(void)
{
    num_states = 4;
    state = NORMAL;
    relief = RAISED;
    inverted_frames = { 1 };

    press_effect = true;

    command_func = DirectCheckBox::command_func;
}

DirectCheckBox::DirectCheckBox(NodePath parent, const std::shared_ptr<Options>& options): DirectCheckBox(new PGButton(""), parent, options, get_class_type())
{
}

DirectCheckBox::DirectCheckBox(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle):
    DirectButton(gui_item, parent, define_options(options), type_handle)
{
    // Call option initialization functions
    if (is_exact_type(type_handle))
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

void DirectCheckBox::command_func(const Event* ev, void* user_data)
{
    const auto& options = std::dynamic_pointer_cast<Options>(reinterpret_cast<DirectCheckBox*>(user_data)->_options);

    options->is_checked = !options->is_checked;

    if (options->is_checked)
        reinterpret_cast<DirectCheckBox*>(user_data)->set_image(options->checked_image);
    else
        reinterpret_cast<DirectCheckBox*>(user_data)->set_image(options->unchecked_image);

    if (options->command)
    {
        // Pass any extra args to command
        options->command(user_data);
    }
}

const std::shared_ptr<DirectCheckBox::Options>& DirectCheckBox::define_options(const std::shared_ptr<Options>& options)
{
    return options;
}

void DirectCheckBox::initialise_options(const std::shared_ptr<Options>& options)
{
    DirectButton::initialise_options(options);

    f_init_ = true;
    f_init_ = false;
}

}
