#include "rppanda/gui/config_rppanda_gui.hpp"

#include "dconfig.h"

#include "render_pipeline/rppanda/gui/direct_button.hpp"
#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rppanda/gui/direct_frame.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_base.hpp"
#include "render_pipeline/rppanda/gui/direct_scroll_bar.hpp"
#include "render_pipeline/rppanda/gui/direct_scrolled_frame.hpp"
#include "render_pipeline/rppanda/gui/direct_slider.hpp"
#include "render_pipeline/rppanda/gui/onscreen_image.hpp"

Configure(config_rppanda_gui);
NotifyCategoryDef(rppanda_gui, "");

ConfigureFn(config_rppanda_gui)
{
    init_librppanda_gui();
}

void init_librppanda_gui()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }

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
