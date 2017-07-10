#include "render_pipeline/rpcore/logger.hpp"

#include <spdlog/spdlog.h>

namespace rpcore {

spdlog::logger* global_logger_ = nullptr;

struct RPLogger::Impl
{
    ~Impl(void);

    void create(const std::string& file_path);

public:
    std::shared_ptr<spdlog::logger> logger_;
};

RPLogger::Impl::~Impl(void)
{
    global_logger_ = nullptr;
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

    global_logger_ = logger_.get();
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
