/**
 * Render Pipeline C++
 *
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

#include "render_pipeline/rpcore/logger.hpp"

#include <spdlog/spdlog.h>

namespace rpcore {

std::shared_ptr<spdlog::logger> global_logger_ = spdlog::stdout_color_mt("rpcpp_default_logger");

class RPLogger::Impl
{
public:
    ~Impl(void);

    void create(const std::string& file_path);

public:
    std::shared_ptr<spdlog::logger> logger_;
};

RPLogger::Impl::~Impl(void)
{
    global_logger_.reset();
}

void RPLogger::Impl::create(const std::string& file_path)
{
    if (logger_)
        return;

    std::vector<spdlog::sink_ptr> sinks;
#ifdef _WIN32
    sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
#else
    sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_sink>(spdlog::sinks::stdout_sink_mt::instance()));
#endif
    if (!file_path.empty())
        sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(file_path, true));

    logger_ = std::make_shared<spdlog::logger>("rpcpp_logger", std::begin(sinks), std::end(sinks));
    logger_->set_pattern("[%H:%M:%S.%e] [%t] [%l] %v");
    logger_->flush_on(spdlog::level::err);

    global_logger_ = logger_;
}

// ************************************************************************************************

RPLogger& RPLogger::get_instance(void)
{
    static RPLogger instance;
    return instance;
}

RPLogger::RPLogger(void): impl_(std::make_unique<Impl>())
{
}

RPLogger::~RPLogger(void) = default;

std::shared_ptr<spdlog::logger> RPLogger::get_internal_logger(void)
{
    return impl_->logger_;
}

void RPLogger::create(const std::string& file_path)
{
    impl_->create(file_path);
}

bool RPLogger::is_created(void) const
{
    return impl_->logger_ != nullptr;
}

}
