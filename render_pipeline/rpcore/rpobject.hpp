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

#include <string>

#include <render_pipeline/rpcore/config.hpp>

namespace rpcore {

class RENDER_PIPELINE_DECL RPObject
{
public:
    static void global_trace(const std::string& context, const std::string& message);
    static void global_debug(const std::string& context, const std::string& message);
    static void global_info(const std::string& context, const std::string& message);
    static void global_warn(const std::string& context, const std::string& message);
    static void global_error(const std::string& context, const std::string& message);

    RPObject(const std::string& name);

    const std::string& get_debug_name(void) const;
    void set_debug_name(const std::string& name);

    void trace(const std::string& message) const;
    void debug(const std::string& message) const;
    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;
    void fatal(const std::string& message) const;

protected:
    std::string debug_name_;
};

// ************************************************************************************************

inline const std::string& RPObject::get_debug_name(void) const
{
    return debug_name_;
}

inline void RPObject::set_debug_name(const std::string& name)
{
    debug_name_ = name;
}

inline void RPObject::trace(const std::string& message) const
{
    global_trace(debug_name_, message);
}

inline void RPObject::debug(const std::string& message) const
{
    global_debug(debug_name_, message);
}

inline void RPObject::info(const std::string& message) const
{
    global_info(debug_name_, message);
}

inline void RPObject::warn(const std::string& message) const
{
    global_warn(debug_name_, message);
}

inline void RPObject::error(const std::string& message) const
{
    global_error(debug_name_, message);
}

}
