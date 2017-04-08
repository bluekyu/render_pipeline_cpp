#pragma once

#include <spdlog/spdlog.h>

#define RPLOG(_lvl, _fmt, ...) (::rpcore::logger_->log(::spdlog::level::_lvl, "[{}] " _fmt, debug_name_, __VA_ARGS__))
#define RPLOG_PIMPL(_lvl, _fmt, ...) (::rpcore::logger_->log(::spdlog::level::_lvl, "[{}] " _fmt, self_.debug_name_, __VA_ARGS__))

namespace rpcore {

extern std::shared_ptr<spdlog::logger> logger_;

}
