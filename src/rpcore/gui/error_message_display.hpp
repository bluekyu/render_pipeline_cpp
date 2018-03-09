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

#include <render_pipeline/rpcore/rpobject.hpp>

class LineStream;

namespace rpcore {

class ErrorMessageDisplay : public RPObject
{
public:
    ErrorMessageDisplay();
    ~ErrorMessageDisplay();

    /** Updates the error display, fetching all new messages from the notify stream. */
    void update();

    /** Adds a new error message. */
    void add_error(const std::string& msg);

    /** Adds a new warning message. */
    void add_warning(const std::string& msg);

    /** Internal method to add a new text to the output. */
    void add_text(const std::string& text, const LVecBase3f& color);

    /** Clears all messages / removes them. */
    void clear_messages();

    void show();

    void hide();

private:
    /** Internal method to init the stream to catch all notify messages. */
    void init_notify();

    int num_errors_ = 0;
    NodePath error_node_;
    std::unique_ptr<LineStream> notify_stream_;
};

// ************************************************************************************************
inline void ErrorMessageDisplay::show()
{
    error_node_.show();
}

inline void ErrorMessageDisplay::hide()
{
    error_node_.hide();
}

}
