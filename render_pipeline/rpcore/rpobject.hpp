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

#include <boost/utility/string_view.hpp>

#include <render_pipeline/rpcore/config.hpp>

namespace rpcore {

class RENDER_PIPELINE_DECL RPObject
{
public:
    static void global_trace(boost::string_view context, boost::string_view message);
    static void global_debug(boost::string_view context, boost::string_view message);
    static void global_info(boost::string_view context, boost::string_view message);
    static void global_warn(boost::string_view context, boost::string_view message);
    static void global_error(boost::string_view context, boost::string_view message);

    RPObject(boost::string_view name);

    const std::string& get_debug_name() const;
    void set_debug_name(boost::string_view name);

    void trace(boost::string_view message) const;
    void debug(boost::string_view message) const;
    void info(boost::string_view message) const;
    void warn(boost::string_view message) const;
    void error(boost::string_view message) const;
    void fatal(boost::string_view message) const;

protected:
    std::string debug_name_;
};

// ************************************************************************************************

inline RPObject::RPObject(boost::string_view name): debug_name_(name.empty() ? "RPObject" : name)
{
}

inline const std::string& RPObject::get_debug_name() const
{
    return debug_name_;
}

inline void RPObject::set_debug_name(boost::string_view name)
{
    debug_name_ = name.to_string();
}

inline void RPObject::trace(boost::string_view message) const
{
    global_trace(debug_name_, message);
}

inline void RPObject::debug(boost::string_view message) const
{
    global_debug(debug_name_, message);
}

inline void RPObject::info(boost::string_view message) const
{
    global_info(debug_name_, message);
}

inline void RPObject::warn(boost::string_view message) const
{
    global_warn(debug_name_, message);
}

inline void RPObject::error(boost::string_view message) const
{
    global_error(debug_name_, message);
}

}
