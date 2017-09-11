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

#include <nodePath.h>

#include <functional>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rppanda {
class DirectSlider;
}

namespace rpcore {

/** This is a simple wrapper around DirectSlider, providing a simpler interface. */
class Slider : public RPObject
{
public:
    struct RENDER_PIPELINE_DECL Default
    {
        static const float size;
        static const float max_value;
        static const float value;
        static const float page_size;
    };

public:
    Slider(float x=0, float y=0, NodePath parent={}, float size=Default::size,
        float min_value=0, float max_value=Default::max_value,
        float value=Default::value, float page_size=Default::page_size,
        const std::function<void(const std::shared_ptr<void>&)>& callback={},
        const std::shared_ptr<void>& extra_args={});

    /** Returns the currently assigned value. */
    float get_value() const;

    /** Sets the value of the slider. */
    void set_value(float value);

    /** Returns a handle to the internally used node. */
    rppanda::DirectSlider* get_node() const;

private:
    PT(rppanda::DirectSlider) node_;
};

// ************************************************************************************************

inline rppanda::DirectSlider* Slider::get_node() const
{
    return node_;
}

}
