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

#include "render_pipeline/rpcore/gui/checkbox_collection.hpp"

#include "render_pipeline/rpcore/gui/checkbox.hpp"
#include "render_pipeline/rppanda/gui/direct_check_box.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rpcore {

CheckboxCollection::CheckboxCollection(): RPObject("CheckboxCollection")
{
}

CheckboxCollection::~CheckboxCollection()
{
    for (auto&& item: items_)
        item->set_collection(nullptr);
}

void CheckboxCollection::add(Checkbox* chb)
{
    if (chb->get_collection())
    {
        error(
            "Can't add checkbox as it already belongs "
            "to another collection!");
        return;
    }

    chb->set_collection(this);
    items_.push_back(chb);
}

void CheckboxCollection::remove(Checkbox* chb)
{
    if (chb->get_collection() != this)
    {
        error(
            "Can't remove the checkbox from this collection as it is not "
            "attached to this collection!");
        return;
    }

    chb->set_collection(nullptr);
    items_.erase(std::find(items_.begin(), items_.end(), chb));
}

void CheckboxCollection::on_checkbox_changed(Checkbox* chb)
{
    for (auto&& item: items_)
    {
        if (item != chb)
        {
            item->set_checked(false);
            item->get_node()->set_state(rppanda::DGG_NORMAL);
        }
    }
}

}
