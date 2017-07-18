#pragma once

#include <render_pipeline/rpcore/gui/draggable_window.hpp>

namespace rpcore {

class RenderPipeline;
class Sprite;
class LabeledCheckbox;
class Slider;

class TexturePreview: public DraggableWindow
{
public:
    TexturePreview(RenderPipeline* pipeline, NodePath parent);

    /** "Presents" a given texture and shows the window. */
    void present(Texture* tex);

private:
    virtual void create_components(void);

    void set_slice(void* extra_args);

    void set_mip(void* extra_args);

    void set_brightness(void* extra_args);

    void set_enable_tonemap(bool arg, void* extra_args);

    RenderPipeline* _pipeline;
    NodePath _content_node;
    Texture* _current_tex;

    std::shared_ptr<Sprite> _preview_image;
    std::shared_ptr<LabeledCheckbox> _tonemap_box;

    std::shared_ptr<Text> _mip_text;
    std::shared_ptr<Text> _slice_text;
    std::shared_ptr<Text> _bright_text;

    std::shared_ptr<Slider> _mip_slider;
    std::shared_ptr<Slider> _slice_slider;
    std::shared_ptr<Slider> _bright_slider;
};

}
