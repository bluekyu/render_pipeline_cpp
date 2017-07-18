/**
 * This is C++ porting codes of direct/src/gui/DirectFrame.py
 */

#include "render_pipeline/rppanda/gui/direct_frame.hpp"

#include <pgItem.h>

#include "render_pipeline/rppanda/gui/onscreen_text.hpp"
#include "render_pipeline/rppanda/gui/onscreen_image.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

const std::type_info& DirectFrame::type_handle_(typeid(DirectFrame));

DirectFrame::Options::Options(void)
{
    num_states = 1;
    state = inactive_init_state;
}

DirectFrame::DirectFrame(NodePath parent, const std::shared_ptr<Options>& options): DirectFrame(new PGItem(""), parent, options, get_class_type())
{
}

DirectFrame::DirectFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle):
    DirectGuiWidget(gui_item, parent, define_options(options), type_handle)
{
    // Call option initialization functions
    if (get_class_type() == type_handle)
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

void DirectFrame::set_text(const std::string& text)
{
    set_text(std::vector<std::string>({ text }));
}

void DirectFrame::set_text(const std::vector<std::string>& text_list)
{
    std::dynamic_pointer_cast<Options>(_options)->text = text_list;

    // Create/destroy components
    for (int i=0; i < _options->num_states; ++i)
    {
        const std::string& component_name = "text" + std::to_string(i);

        // If fewer items specified than numStates,
        // just repeat last item
        std::string text;
        if (i >= text.size())
            text = text_list.empty() ? std::string("") : text_list.back();
        else
            text = text_list[i];

        if (has_component(component_name))
        {
            if (text.empty())
            {
                // Destroy component
                boost::any_cast<OnscreenText&>(get_component(component_name)).destroy();
                remove_component(component_name);
            }
            else
            {
                boost::any_cast<OnscreenText&>(get_component(component_name)).set_text(text);
            }
        }
        else
        {
            if (text.empty())
                return;
            else
            {
                OnscreenText::Parameters params;
                params.text = text;
                params.style = OnscreenText::Style::plain;
                params.parent = _state_node_path.at(i);
                params.scale = 1;
                params.may_change = std::dynamic_pointer_cast<Options>(_options)->text_may_change;
                params.sort = TEXT_SORT_INDEX;
                create_component(component_name, boost::any(OnscreenText(params)));
            }
        }
    }
}

void DirectFrame::set_image(const std::vector<std::shared_ptr<ImageInput>>& images)
{
    std::vector<std::shared_ptr<ImageInput>> image_list;

    // Passed in None
    if (images.empty())
    {
        for (int k = 0; k < _options->num_states; ++k)
            image_list.push_back(nullptr);
    }
    // Passed in a single node path, make a tuple out of it
    else if (images.size() == 1)
    {
        for (int k = 0; k < _options->num_states; ++k)
            image_list.push_back(images[0]);
    }
    // Assume its a list of node paths
    else
    {
        image_list = images;
    }

    // Create/destroy components
    for (int i = 0; i < _options->num_states; ++i)
    {
        const std::string& component_name = "image" + std::to_string(i);

        // If fewer items specified than numStates,
        // just repeat last item
        std::shared_ptr<ImageInput> image;
        if (i >= image_list.size())
            image = image_list.back();
        else
            image = image_list[i];

        if (has_component(component_name))
        {
            if (!image || image->is_none())
            {
                boost::any_cast<OnscreenImage&>(get_component(component_name)).destroy();
                remove_component(component_name);
            }
            else
            {
                boost::any_cast<OnscreenImage&>(get_component(component_name)).set_image(image);
            }
        }
        else
        {
            if (!image || image->is_none())
            {
                return;
            }
            else
            {
                create_component(component_name, boost::any(
                    OnscreenImage(image, _state_node_path.at(i), IMAGE_SORT_INDEX)
                ));
            }
        }
    }

    const auto& opt = std::dynamic_pointer_cast<Options>(_options);
    opt->image.clear();
    for (auto image: images)
        opt->image.push_back(image);
}

const std::shared_ptr<DirectFrame::Options>& DirectFrame::define_options(const std::shared_ptr<Options>& options)
{
    return options;
}

void DirectFrame::initialise_options(const std::shared_ptr<Options>& options)
{
    DirectGuiWidget::initialise_options(options);

    _f_init = true;
    set_text(options->text);
    set_image(options->image);
    _f_init = false;
}

}
