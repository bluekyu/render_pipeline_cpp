/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2018 Center of Human-centered Interaction for Coexistence.
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

#pragma once

#include <render_pipeline/rpcore/gui/draggable_window.hpp>

namespace rpcore {

class RenderPipeline;
class CheckboxCollection;
class LabeledCheckbox;

/** Window which offers the user to select a render mode to apply */
class RenderModeSelector : public DraggableWindow
{
public:
    RenderModeSelector(RenderPipeline* pipeline, NodePath parent);
    ~RenderModeSelector() override;

    /** Toggles the visibility of this windows. */
    void toggle();

private:
    /** Internal method to init the components. */
    void create_components();

    /** Populates the windows content. */
    void populate_content();

    /** Callback which gets called when a render mode got selected. */
    void set_render_mode(const std::string& mode_id, bool special, bool value);

    RenderPipeline* pipeline_;
    std::string selected_mode_;
    NodePath content_node_;
    std::unique_ptr<CheckboxCollection> collection_;
    std::vector<std::unique_ptr<LabeledCheckbox>> boxes_;
};

inline void RenderModeSelector::toggle()
{
    if (visible_)
    {
        hide();
    }
    else
    {
        populate_content();
        show();
    }
}

}
