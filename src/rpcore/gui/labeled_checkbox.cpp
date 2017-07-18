#include "render_pipeline/rpcore/gui/labeled_checkbox.hpp"

#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rpcore {

LabeledCheckbox::LabeledCheckbox(const Parameters& params): RPObject("LabeledCheckbox")
{
    if (params.enabled)
        _text_color = params.text_color;
    else
        _text_color = LVecBase3f(1.0f, 0.0f, 0.28f);

    _checkbox = new Checkbox(params);

    Text::Parameters text_params;
    text_params.text = params.text;
    text_params.parent = params.parent;
    text_params.x = params.x + 26;
    text_params.y = params.y + 9;
    text_params.size = params.text_size;
    text_params.align = "left";
    text_params.color = _text_color;
    text_params.may_change = true;
    _text = new Text(text_params);

    if (params.enabled)
    {
        _checkbox->get_node()->bind(rppanda::WITHIN, on_node_enter, this);
        _checkbox->get_node()->bind(rppanda::WITHOUT, on_node_leave, this);
    }
}

LabeledCheckbox::~LabeledCheckbox(void)
{
    delete _text;
    delete _checkbox;
}

void LabeledCheckbox::on_node_enter(const Event* ev, void* user_data)
{
    LabeledCheckbox* lc = reinterpret_cast<LabeledCheckbox*>(user_data);
    lc->_text->get_node().set_fg(LColorf(lc->_text_color[0] + 0.1f, lc->_text_color[1] + 0.1f,
        lc->_text_color[2] + 0.1f, 1.0f));
}

void LabeledCheckbox::on_node_leave(const Event* ev, void* user_data)
{
    LabeledCheckbox* lc = reinterpret_cast<LabeledCheckbox*>(user_data);
    lc->_text->get_node().set_fg(LColorf(lc->_text_color[0], lc->_text_color[1], lc->_text_color[2], 1.0f));
}

}
