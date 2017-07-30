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

#include "rpcore/gui/error_message_display.hpp"

#include <lineStream.h>
#include <pnotify.h>

#include <boost/algorithm/string.hpp>

#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rpcore {

ErrorMessageDisplay::ErrorMessageDisplay(void): RPObject("ErrorMessageDisplay")
{
    _error_node = Globals::base->get_pixel_2d().attach_new_node("ErrorDisplay");
    _error_node.set_z(-180);
}

ErrorMessageDisplay::~ErrorMessageDisplay(void)
{
    delete _notify_stream;
}

void ErrorMessageDisplay::update(void)
{
    if (!_notify_stream)
        init_notify();

    while (_notify_stream->is_text_available())
    {
        const std::string line(_notify_stream->get_line());
        if (line.find("warning") != std::string::npos)
        {
            RPObject::global_warn("Panda3d", line);
        }
        else if (line.find("error") != std::string::npos)
        {
            RPObject::global_error("Panda3d", line);
            add_error(line);
        }
        else
        {
            RPObject::global_debug("Panda3d", line);
        }
    }
}

void ErrorMessageDisplay::add_error(const std::string& msg)
{
    add_text(msg, LVecBase3f(1.0f, 0.2f, 0.2f));
}

void ErrorMessageDisplay::add_warning(const std::string& msg)
{
    add_text(msg, LVecBase3f(1.0f, 1.0f, 0.2f));
}

void ErrorMessageDisplay::add_text(const std::string& text, const LVecBase3f& color)
{
    Text::Parameters params;
    params.text = boost::trim_copy(text);
    params.parent = _error_node;
    params.x = Globals::native_resolution.get_x() - 30;
    params.y = _num_errors * 23;
    params.size = 12.0f;
    params.align = "right";
    params.color = color;
    Text error_text(params);

    _num_errors += 1;

    if (_num_errors > 30)
    {
        clear_messages();
        add_error("Error count exceeded. Cleared errors ..");
    }
}

void ErrorMessageDisplay::clear_messages(void)
{
    _error_node.node()->remove_all_children();
    _num_errors = 0;
}

void ErrorMessageDisplay::init_notify(void)
{
    _notify_stream = new LineStream;
    Notify::ptr()->set_ostream_ptr(_notify_stream, false);
}

}
