/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2018 Center of Human-centered Interaction for Coexistence.
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

namespace rpcore {

class Checkbox;

/**
 * This is a container for multiple Checkboxes, controlling that
 * only one checkbox of this collection is checked at one time
 * (like a radio-button)
 */
class RENDER_PIPELINE_DECL CheckboxCollection : public RPObject
{
public:
    CheckboxCollection();

    ~CheckboxCollection();

    /** Adds a Checkbox to this collection. */
    void add(Checkbox* chb);

    /** Removes a checkbox from this collection. */
    void remove(Checkbox* chb);

    /** Internal callback when a checkbox got changed. */
    void on_checkbox_changed(Checkbox* chb);

private:
    std::vector<Checkbox*> items_;
};

}
