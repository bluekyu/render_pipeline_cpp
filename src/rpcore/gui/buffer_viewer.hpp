/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
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

#pragma once

#include <render_pipeline/rpcore/gui/draggable_window.hpp>

namespace rppanda {
class DirectScrolledFrame;
class DirectFrame;
}

namespace rpcore {

class TexturePreview;
class RenderStage;
class LabeledCheckbox;

/** This class provides a view into the buffers to inspect them . */
class BufferViewer : public DraggableWindow
{
    enum class EntryID: int
    {
        TEXTURE = 0,
        RENDER_TARGET,
    };

public:
    using EntryType = std::pair<void*, EntryID>;

    BufferViewer(NodePath parent);
    virtual ~BufferViewer();

    void toggle();

    std::vector<EntryType> get_entries() const;

    /** Returns the amount of attached stages, and also the memory consumed in MiB in a tuple. */
    std::pair<size_t, int> get_stage_information() const;

private:
    /** Creates the window components. */
    void create_components() override;

    /** Sets whether images and textures will be shown. */
    void set_show_images(bool arg);

    /** Sets the maximum scroll height in the content frame */
    void set_scroll_height(int height);

    /** Removes all components of the buffer viewer. */
    void remove_components();

    /** Collects all entries, extracts their images and re-renders the window. */
    void perform_update();

    /** Internal method when a texture is hovered. */
    void on_texture_hovered(rppanda::DirectFrame* hover_frame);

    /** Internal method when a texture is blurred. */
    void on_texture_blurred(rppanda::DirectFrame* hover_frame);

    /** Internal method when a texture is clicked. */
    void on_texture_clicked(Texture* tex_handle);

    /** Renders the stages to the window. */
    void render_stages();

    int _scroll_height = 3000;
    bool _display_images = false;
    PT(rppanda::DirectScrolledFrame) _content_frame;
    NodePath _content_node;
    std::unique_ptr<LabeledCheckbox> _chb_show_images;

    std::vector<std::pair<Texture*, EntryID>> _stages;
    std::unique_ptr<TexturePreview> _tex_preview;

    std::vector<rppanda::DirectFrame*> frame_hovers_;
};

}
