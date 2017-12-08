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
 * This is C++ porting codes of direct/src/gui/DirectFrame.py
 */

#include "render_pipeline/rppanda/gui/direct_frame.hpp"

#include <pgItem.h>

#include "render_pipeline/rppanda/gui/onscreen_text.hpp"
#include "render_pipeline/rppanda/gui/onscreen_image.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

TypeHandle DirectFrame::type_handle_;

DirectFrame::Options::Options()
{
    num_states = 1;
    state = inactive_init_state;
}

DirectFrame::DirectFrame(NodePath parent, const std::shared_ptr<Options>& options): DirectFrame(new PGItem(""), parent, options, get_class_type())
{
}

DirectFrame::DirectFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle):
    DirectGuiWidget(gui_item, parent, define_options(options), type_handle)
{
    // Call option initialization functions
    if (is_exact_type(type_handle))
    {
        initialise_options(options);
        frame_initialise_func();
    }
}

const std::vector<std::string>& DirectFrame::get_text() const
{
    return std::dynamic_pointer_cast<Options>(_options)->text;
}

void DirectFrame::prepare_text(const std::string& text)
{
    std::dynamic_pointer_cast<Options>(_options)->text = std::vector<std::string>({ text });
}

void DirectFrame::prepare_text(const std::vector<std::string>& text_list)
{
    std::dynamic_pointer_cast<Options>(_options)->text = text_list;
}

void DirectFrame::set_text()
{
    const auto& text_list = std::dynamic_pointer_cast<Options>(_options)->text;

    // Create/destroy components
    for (int i = 0; i < _options->num_states; ++i)
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
                create_component(component_name, boost::any(OnscreenText(
                    text, OnscreenText::Style::plain, LVecBase2(0), 0, LVecBase2(1),
                    {}, {}, {}, OnscreenText::Default::shadow_offset, {},
                    {}, {}, {}, false, nullptr, _state_node_path.at(i), TEXT_SORT_INDEX,
                    std::dynamic_pointer_cast<Options>(_options)->text_may_change)));
            }
        }
    }
}

const std::vector<std::shared_ptr<ImageInput>>& DirectFrame::get_image() const
{
    return std::dynamic_pointer_cast<Options>(_options)->image;
}

void DirectFrame::prepare_image(const std::shared_ptr<ImageInput>& image)
{
    std::dynamic_pointer_cast<Options>(_options)->image = std::vector<std::shared_ptr<ImageInput>>({ image });
}

void DirectFrame::prepare_image(const std::vector<std::shared_ptr<ImageInput>>& images)
{
    std::dynamic_pointer_cast<Options>(_options)->image = images;
}

void DirectFrame::set_image()
{
    const auto& images = std::dynamic_pointer_cast<Options>(_options)->image;

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
                boost::any_cast<PT(OnscreenImage)>(get_component(component_name))->destroy();
                remove_component(component_name);
            }
            else
            {
                boost::any_cast<PT(OnscreenImage)>(get_component(component_name))->set_image(image);
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
                PT(OnscreenImage) onscreen_image = new OnscreenImage(image, _state_node_path.at(i), IMAGE_SORT_INDEX);
                create_component(component_name, boost::any(onscreen_image));
            }
        }
    }
}

const std::shared_ptr<DirectFrame::Options>& DirectFrame::define_options(const std::shared_ptr<Options>& options)
{
    return options;
}

void DirectFrame::initialise_options(const std::shared_ptr<Options>& options)
{
    DirectGuiWidget::initialise_options(options);

    f_init_ = true;
    set_text(options->text);
    set_image(options->image);
    f_init_ = false;
}

}
