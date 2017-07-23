#include "render_pipeline/rpcore/gui/slider.hpp"

#include "render_pipeline/rppanda/gui/direct_slider.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rpcore {

Slider::Slider(const Parameters& params): RPObject("Slider")
{
    // TODO: implement
    // Scale has to be 2.0, otherwise there will be an error.
    auto options = std::make_shared<rppanda::DirectSlider::Options>();
    options->pos = LVecBase3(params.size * 0.5 + params.x, 1, -params.y);
    options->range = LVecBase2(params.min_value, params.max_value);
    options->value = params.value;
    options->page_size = params.page_size;
    options->scale = 2.0f;
    options->command=params.callback;
    options->extra_args = params.extra_args;
    options->frame_color = LColor(0.0, 0.0, 0.0, 1);
    options->frame_size = LVecBase4(-params.size * 0.25, params.size * 0.25, -5, 5);
    options->relief = rppanda::FLAT;
    options->thumb_options->frame_color = LColor(0.35, 0.53, 0.2, 1.0);
    options->thumb_options->relief = rppanda::FLAT;
    options->thumb_options->frame_size = LVecBase4(-2.5, 2.5, -5.0, 5.0);
    node_ = new rppanda::DirectSlider(params.parent, options);
}

float Slider::get_value(void) const
{
    return node_->get_value();
}

void Slider::set_value(float value)
{
    node_->set_value(value);
}

}
