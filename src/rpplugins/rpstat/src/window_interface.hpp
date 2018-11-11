/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <string>

#include <imgui.h>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

namespace rpcore {
class RenderPipeline;
}

namespace rpplugins {

class RPStatPlugin;

class WindowInterface : public rppanda::DirectObject
{
public:
    static constexpr const char* SHOW_WINDOW_EVENT_NAME_PREFIX = "rpstat-window-show";

    static void send_show_event(const std::string& unique_id);

public:
    WindowInterface(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, const std::string& title);
    WindowInterface(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, const std::string& title, const std::string& unique_id);

    virtual ~WindowInterface();

    virtual void draw();
    virtual void draw_contents() = 0;

    virtual void show();
    virtual void hide();

    const std::string& get_unique_id() const;

protected:
    static size_t window_count_;

    RPStatPlugin& plugin_;
    rpcore::RenderPipeline& pipeline_;

    const size_t window_id_;
    const std::string unique_id_;
    std::string title_;
    bool is_open_ = false;
    ImGuiWindowFlags window_flags_ = 0;
};

// ************************************************************************************************

inline void WindowInterface::show()
{
    is_open_ = true;
}

inline void WindowInterface::hide()
{
    is_open_ = false;
}

inline const std::string& WindowInterface::get_unique_id() const
{
    return unique_id_;
}

}
