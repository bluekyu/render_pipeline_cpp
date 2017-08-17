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

#include <render_pipeline/rppanda/gui/onscreen_image.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpcore {

/**
 * Simple wrapper arround OnscreenImage, providing a simpler interface.
 */
class Sprite: public RPObject
{
public:
    /**
     * Creates a new image, taking (x,y) as topleft coordinates.
     * When near_filter is set to true, a near filter will be set to the
     * texture passed.This provides sharper images.
     * When any_filter is set to false, the passed image won't be modified at
     * all.This enables you to display existing textures, otherwise the
     * texture would get a near filter in the 3D View, too.
     */
    Sprite(Texture* image, int w, int h, NodePath parent={}, float x=0.0f, float y=0.0f,
        bool transparent=true, bool near_filter=true, bool any_filter=true);
    Sprite(Texture* image, NodePath parent={}, float x=0.0f, float y=0.0f,
        bool transparent=true, bool near_filter=true, bool any_filter=true);
    Sprite(const std::string& image_path, int w, int h, NodePath parent={}, float x=0.0f,
        float y=0.0f, bool transparent=true, bool near_filter=true, bool any_filter=true);
    Sprite(const std::string& image_path, NodePath parent={}, float x=0.0f, float y=0.0f,
        bool transparent=true, bool near_filter=true, bool any_filter=true);

    /** Returns a pos interval, this is a wrapper around NodePath.posInterval. */
    const LVecBase3f& get_initial_pos() const;

    /** Returns the width of the image in pixels. */
    int get_width() const;

    /** Returns the height of the image in pixels. */
    int get_height() const;

    /** Sets the position. */
    void set_pos(float x, float y);

    /** Sets a shader to be used for rendering the image. */
    void set_shader(const Shader* shader, int priority=0);

    /** Sets a shader input on the image. */
    void set_shader_input(const ShaderInput& inp);

    /** Hides the image. */
    void hide();

    /** Shows the image if it was previously hidden. */
    void show();

    /** Returns whether the image is hidden. */
    bool is_hidden() const;

    rppanda::OnscreenImage* get_node() const;

private:
    void init(Texture* image, int w, int h, NodePath parent, float x, float y,
        bool transparent, bool near_filter, bool any_filter);
    LVecBase3f translate_pos(float x, float y) const;

    int _width;
    int _height;
    LVecBase3f _initial_pos;

    PT(rppanda::OnscreenImage) node_;
};

// ************************************************************************************************
inline Sprite::Sprite(Texture* image, NodePath parent, float x, float y,
    bool transparent, bool near_filter, bool any_filter):
    Sprite(image, 10, 10, parent, x, y, transparent, near_filter, any_filter)
{
}

inline LVecBase3f Sprite::translate_pos(float x, float y) const
{
    return LVecBase3f(x + _width / 2.0f, 1, -y - _height / 2.0f);
}

inline const LVecBase3f& Sprite::get_initial_pos() const
{
    return _initial_pos;
}

inline int Sprite::get_width() const
{
    return _width;
}

inline int Sprite::get_height() const
{
    return _height;
}

inline void Sprite::set_pos(float x, float y)
{
    node_->set_pos(translate_pos(x, y));
}

inline void Sprite::set_shader(const Shader* shader, int priority)
{
    node_->set_shader(shader, priority);
}

inline void Sprite::set_shader_input(const ShaderInput& inp)
{
    node_->set_shader_input(inp);
}

inline void Sprite::hide()
{
    node_->hide();
}

inline void Sprite::show()
{
    node_->show();
}

inline bool Sprite::is_hidden() const
{
    return node_->is_hidden();
}

inline rppanda::OnscreenImage* Sprite::get_node() const
{
    return node_;
}

}
