#include "render_pipeline/rpcore/rpobject.h"

#include "rpcore/logger.hpp"

namespace rpcore {

void RPObject::global_debug(const std::string& context, const std::string& message)
{
    logger_->debug("[{}] {}", context, message);
}

void RPObject::global_info(const std::string& context, const std::string& message)
{
    logger_->info("[{}] {}", context, message);
}

void RPObject::global_warn(const std::string& context, const std::string& message)
{
    logger_->warn("[{}] {}", context, message);
}

void RPObject::global_error(const std::string& context, const std::string& message)
{
    logger_->error("[{}] {}", context, message);
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
