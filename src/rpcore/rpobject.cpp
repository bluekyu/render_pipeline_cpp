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

#include <spdlog/logger.h>

namespace rpcore {

extern std::shared_ptr<spdlog::logger> global_logger_;

void RPObject::global_trace(const std::string& context, const std::string& message)
{
    global_logger_->trace("[{}] {}", context, message);
}

void RPObject::global_debug(const std::string& context, const std::string& message)
{
    global_logger_->debug("[{}] {}", context, message);
}

void RPObject::global_info(const std::string& context, const std::string& message)
{
    global_logger_->info("[{}] {}", context, message);
}

void RPObject::global_warn(const std::string& context, const std::string& message)
{
    global_logger_->warn("[{}] {}", context, message);
}

void RPObject::global_error(const std::string& context, const std::string& message)
{
    global_logger_->error("[{}] {}", context, message);
}

RPObject::RPObject(const std::string& name)
{
    if (name.empty())
        debug_name_ = "RPObject";
    else
        debug_name_ = name;
}

void RPObject::fatal(const std::string& message) const
{
    error(message);
    std::exit(0);
}

}
