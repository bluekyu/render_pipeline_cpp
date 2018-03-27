# Panda3D GUI

You can see full source codes in https://github.com/bluekyu/rpcpp_samples/tree/master/snippets/GUIs

## Text
```cpp
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>

MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // create OnscreenText using constructor
    button_text_ = rppanda::OnscreenText(
        "Button Pressed: 0", rppanda::OnscreenText::Default::style, LVecBase2(1.0, 0.07f), 0,
        LVecBase2(0.05f), LColor(1, 1, 1, 1), {},
        LColor(0, 0, 0, 1), rppanda::OnscreenText::Default::shadow_offset, {},
        {}, {}, {}, false,
        {}, ui_root_);

    // create OnscreenText using methods
    checkbox_text_ = rppanda::OnscreenText("CheckBox Status: Off");
    checkbox_text_.reparent_to(ui_root_);
    checkbox_text_.set_pos(LVecBase2(1.0f, -0.15f));
    checkbox_text_.set_scale(LVecBase2(0.04f));
    checkbox_text_.set_fg(LColor(1, 1, 1, 1));
    checkbox_text_.set_shadow(LColor(0, 0, 0, 1));
    checkbox_text_.set_align(TextProperties::A_left);
}
```

## Button
```cpp
#include <render_pipeline/rppanda/gui/direct_button.hpp>

MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // create button
    auto button_options = std::make_shared<rppanda::DirectButton::Options>();
    button_options->pos = LVecBase3(1.0f, 0.0f, 0.0f);
    button_options->text ={ "Button" };
    button_options->scale = 0.05f;
    button_options->pad = LVecBase2(0.2f, 0.2f);
    button_options->command = std::bind(&MainUI::button_on_clicked, this);
    button_ = new rppanda::DirectButton(ui_root_, button_options);
}

void MainUI::button_on_clicked()
{
    static size_t count = 0;
    button_text_.set_text("Button Pressed: " + std::to_string(++count));
}
```

## Slider
```cpp
#include <render_pipeline/rppanda/gui/direct_slider.hpp>

MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // create slider
    auto slider_options = std::make_shared<rppanda::DirectSlider::Options>();
    slider_options->pos = LVecBase3(1.0f, 0.0f, -0.07f);
    slider_options->scale = 0.4f;
    slider_options->value = pipeline_.get_daytime_mgr()->get_time();
    slider_options->command = std::bind(&MainUI::slider_on_changed, this);
    slider_ = new rppanda::DirectSlider(ui_root_, slider_options);
}

void MainUI::slider_on_changed()
{
    pipeline_.get_daytime_mgr()->set_time(slider_->get_value());
}
```

## Check Box
```cpp
#include <render_pipeline/rppanda/gui/direct_check_box.hpp>

MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // create checkbox
    auto checked_img = rpcore::RPLoader::load_texture("/$$rp/data/gui/checkbox_checked.png");
    auto unchecked_img = rpcore::RPLoader::load_texture("/$$rp/data/gui/checkbox_default.png");

    auto checkbox_options = std::make_shared<rppanda::DirectCheckBox::Options>();
    checkbox_options->pos = LVecBase3f(0.95f, 0.0f, -0.15f);
    checkbox_options->scale = LVecBase3f(0.03f);
    checkbox_options->checked_image = std::make_shared<rppanda::ImageInput>(checked_img);
    checkbox_options->unchecked_image = std::make_shared<rppanda::ImageInput>(unchecked_img);
    checkbox_options->image ={ std::make_shared<rppanda::ImageInput>(unchecked_img) };
    checkbox_options->checkbox_command = std::bind(&MainUI::checkbox_on_clicked, this, std::placeholders::_1);
    checkbox_ = new rppanda::DirectCheckBox(ui_root_, checkbox_options);
}

void MainUI::checkbox_on_clicked(bool status)
{
    if (status)
        checkbox_text_.set_text("CheckBox Status: On");
    else
        checkbox_text_.set_text("CheckBox Status: Off");
}
```

## Text Entry
```cpp
#include <render_pipeline/rppanda/gui/direct_entry.hpp>

MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // create OnscreenText using methods
    entry_text_ = rppanda::OnscreenText("Entry: ");
    entry_text_.reparent_to(ui_root_);
    entry_text_.set_pos(LVecBase2(0.8f, -0.24f));
    entry_text_.set_scale(LVecBase2(0.04f));
    entry_text_.set_fg(LColor(1, 1, 1, 1));
    entry_text_.set_shadow(LColor(0, 0, 0, 1));
    entry_text_.set_align(TextProperties::A_left);

    auto entry_options = std::make_shared<rppanda::DirectEntry::Options>();
    entry_options->initial_text = "Text Entry";
    entry_options->command = std::bind(&MainUI::entry_on_changed, this, std::placeholders::_1);
    entry_options->pos = LVecBase3(0.8f, 0, -0.32f);
    entry_options->scale = LVecBase3(0.04f);
    entry_ = new rppanda::DirectEntry(ui_root_, entry_options);
}

void MainUI::entry_on_changed(const std::string& text)
{
    entry_text_.set_text("Entry: " + text);
}
```
