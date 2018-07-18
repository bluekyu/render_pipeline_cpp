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

#include "render_pipeline/rpcore/logger_manager.hpp"

#include <virtualFileSystem.h>

#include <spdlog/spdlog.h>

#include "render_pipeline/rppanda/util/filesystem.hpp"

namespace rpcore {

spdlog::logger* global_logger_ = nullptr;

LoggerManager& LoggerManager::get_instance()
{
    static LoggerManager instance;
    return instance;
}

LoggerManager::LoggerManager()
{
}

LoggerManager::~LoggerManager()
{
    clear();
}

spdlog::logger* LoggerManager::get_logger() const
{
    return logger_.get();
}

void LoggerManager::create(const Filename& file_path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (logger_)
        return;

    std::vector<spdlog::sink_ptr> sinks;
#ifdef _WIN32
    sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
#else
    sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
#endif

    std::string err_msg;
    if (!file_path.empty())
    {
        if (file_path.is_directory())
        {
            err_msg = "[LoggerManager] file_path is not file.";
        }
        else
        {
            auto dir = Filename(file_path.get_dirname());

            VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
            if (!vfs->is_directory(dir))
                vfs->make_directory_full(dir);

            sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(rppanda::convert_path(file_path).string(), true));
        }
    }

    logger_ = std::make_shared<spdlog::logger>("render_pipeline", std::begin(sinks), std::end(sinks));
    global_logger_ = logger_.get();

    logger_->set_pattern("[%H:%M:%S.%e] [%t] [%l] %v");
    logger_->flush_on(spdlog::level::err);

    if (!err_msg.empty())
        logger_->error(err_msg);

    logger_->debug("LoggerManager created logger");
}

bool LoggerManager::is_created() const
{
    return logger_ != nullptr;
}

void LoggerManager::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (logger_)
    {
        logger_->debug("LoggerManager will drop logger");

        logger_.reset();
        global_logger_ = nullptr;
    }
}

}
