# Panda3D GUI

You can see full source codes in https://github.com/bluekyu/rpcpp_snippets/tree/master/GUIs.

## Text
```cpp
MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // ...
    
    // create OnscreenText using constructor
    button_text_ = rppanda::OnscreenText(
        "Button Pressed: 0", rppanda::OnscreenText::Default::style, LVecBase2(1.0, 0.07f), 0,
        LVecBase2(0.05f), LColor(1, 1, 1, 1), {},
        LColor(0, 0, 0, 1), rppanda::OnscreenText::Default::shadow_offset, {},
        {}, {}, {}, false,
        {}, ui_root_);

    // ...

    // create OnscreenText using methods
    checkbox_text_ = rppanda::OnscreenText("CheckBox Status: Off");
    checkbox_text_.reparent_to(ui_root_);
    checkbox_text_.set_pos(LVecBase2(1.0f, -0.15f));
    checkbox_text_.set_scale(LVecBase2(0.04f));
    checkbox_text_.set_fg(LColor(1, 1, 1, 1));
    checkbox_text_.set_shadow(LColor(0, 0, 0, 1));
    checkbox_text_.set_align(TextProperties::A_left);

    // ...
}
```

## Button
```cpp
MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // ...

    // create button
    auto button_options = std::make_shared<rppanda::DirectButton::Options>();
    button_options->pos = LVecBase3(1.0f, 0.0f, 0.0f);
    button_options->text ={ "Button" };
    button_options->scale = 0.05f;
    button_options->pad = LVecBase2(0.2f, 0.2f);
    button_options->command = std::bind(&MainUI::button_on_clicked, this, std::placeholders::_1);
    button_ = new rppanda::DirectButton(ui_root_, button_options);

    // ...
}

void MainUI::button_on_clicked(const std::shared_ptr<void>& param)
{
    static size_t count = 0;
    button_text_.set_text("Button Pressed: " + std::to_string(++count));
}
```

## Slider
```cpp
MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // ...

    // create slider
    auto slider_options = std::make_shared<rppanda::DirectSlider::Options>();
    slider_options->pos = LVecBase3(1.0f, 0.0f, -0.07f);
    slider_options->scale = 0.4f;
    slider_options->value = pipeline_.get_daytime_mgr()->get_time();
    slider_options->command = std::bind(&MainUI::slider_on_changed, this, std::placeholders::_1);
    slider_ = new rppanda::DirectSlider(ui_root_, slider_options);

    // ...
}

void MainUI::slider_on_changed(const std::shared_ptr<void>& param)
{
    pipeline_.get_daytime_mgr()->set_time(slider_->get_value());
}
```

## Check Box
```cpp
MainUI::MainUI(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // ...

    // create checkbox
    auto checked_img = rpcore::RPLoader::load_texture("/$$rp/data/gui/checkbox_checked.png");
    auto unchecked_img = rpcore::RPLoader::load_texture("/$$rp/data/gui/checkbox_default.png");

    auto checkbox_options = std::make_shared<rppanda::DirectCheckBox::Options>();
    checkbox_options->pos = LVecBase3f(0.95f, 0.0f, -0.15f);
    checkbox_options->scale = LVecBase3f(0.03f);
    checkbox_options->checked_image = std::make_shared<rppanda::ImageInput>(checked_img);
    checkbox_options->unchecked_image = std::make_shared<rppanda::ImageInput>(unchecked_img);
    checkbox_options->image ={ std::make_shared<rppanda::ImageInput>(unchecked_img) };
    checkbox_options->command = std::bind(&MainUI::checkbox_on_clicked, this, std::placeholders::_1);
    checkbox_ = new rppanda::DirectCheckBox(ui_root_, checkbox_options);
}

void MainUI::checkbox_on_clicked(const std::shared_ptr<void>& param)
{
    if (checkbox_->is_checked())
        checkbox_text_.set_text("CheckBox Status: On");
    else
        checkbox_text_.set_text("CheckBox Status: Off");
}
```
