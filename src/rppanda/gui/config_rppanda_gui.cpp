/**
 * Render Pipeline C++
 *
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
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

#include "rppanda/gui/config_rppanda_gui.hpp"

#include "render_pipeline/rppanda/gui/direct_button.hpp"
#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rppanda/gui/direct_frame.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_base.hpp"
#include "render_pipeline/rppanda/gui/direct_scroll_bar.hpp"
#include "render_pipeline/rppanda/gui/direct_scrolled_frame.hpp"
#include "render_pipeline/rppanda/gui/direct_slider.hpp"
#include "render_pipeline/rppanda/gui/onscreen_image.hpp"

ConfigureDef(config_rppanda_gui);
NotifyCategoryDef(rppanda_gui, "");

ConfigureFn(config_rppanda_gui)
{
    init_librppanda_gui();
}

void init_librppanda_gui()
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    rppanda::DirectGuiBase::init_type();
    rppanda::DirectGuiWidget::init_type();
    rppanda::DirectFrame::init_type();
    rppanda::DirectButton::init_type();
    rppanda::DirectCheckBox::init_type();
    rppanda::DirectScrollBar::init_type();
    rppanda::DirectScrolledFrame::init_type();
    rppanda::DirectSlider::init_type();
    rppanda::OnscreenImage::init_type();
}
