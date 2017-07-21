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
