#pragma once

#include <spdlog/logger.h>

#include <render_pipeline/rpcore/config.h>

namespace rpcore {

class RENDER_PIPELINE_DECL RPLogger
{
public:
    static RPLogger& get_instance(void);

    RPLogger(void);
    ~RPLogger(void);

    std::shared_ptr<spdlog::logger> get_internal_logger(void);

    /**
     * Create internal logger.
     * If @p file_path is empty, file logging will be disabled.
     */
    void create(const std::string& file_path);

    bool is_created(void) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
