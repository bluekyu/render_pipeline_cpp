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

#include "render_pipeline/rpcore/rpobject.hpp"

#include <spdlog/tweakme.h>
#include <spdlog/logger.h>
#include <spdlog/fmt/ostr.h>

namespace rpcore {

extern spdlog::logger* global_logger_;

void RPObject::global_trace(boost::string_view context, boost::string_view message)
{
    global_logger_->trace("[{}] {}", context, message);
}

void RPObject::global_debug(boost::string_view context, boost::string_view message)
{
    global_logger_->debug("[{}] {}", context, message);
}

void RPObject::global_info(boost::string_view context, boost::string_view message)
{
    global_logger_->info("[{}] {}", context, message);
}

void RPObject::global_warn(boost::string_view context, boost::string_view message)
{
    global_logger_->warn("[{}] {}", context, message);
}

void RPObject::global_error(boost::string_view context, boost::string_view message)
{
    global_logger_->error("[{}] {}", context, message);
}

void RPObject::fatal(boost::string_view message) const
{
    error(message);
    std::exit(0);
}

}
