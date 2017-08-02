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
class DirectCheckBox;
}

namespace rpcore {

/**
 * This is a wrapper around DirectCheckBox, providing a simpler interface
 * and better visuals.
 */
class RENDER_PIPELINE_DECL Checkbox: public RPObject
{
public:
    Checkbox(NodePath parent={}, float x=0, float y=0, const std::function<void(bool, const std::shared_ptr<void>&)>& callback={},
        const std::shared_ptr<void>& extra_args={}, bool radio=false, int expand_width=100, bool checked=false, bool enabled=true);

    /** Returns whether the node is currently checked. */
    bool is_checked(void) const;

    /** Returns a handle to the internally used node. */
    rppanda::DirectCheckBox* get_node(void) const;

    /** Internal method when another checkbox in the same radio group changed it's value. */
    void update_status(void* args);

    /** Internal method to check/uncheck the checkbox. */
    void set_checked(bool val, bool do_callback=true);

private:
    PT(rppanda::DirectCheckBox) node_;

    std::function<void(bool, const std::shared_ptr<void>&)> callback_;
    std::shared_ptr<void> extra_args_;
};

inline rppanda::DirectCheckBox* Checkbox::get_node(void) const
{
    return node_;
}

}
