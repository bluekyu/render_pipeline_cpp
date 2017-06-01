#pragma once

#include <string>

#include <render_pipeline/rpcore/config.hpp>

namespace rpcore {

class RENDER_PIPELINE_DECL RPObject
{
public:
    static void global_trace(const std::string& context, const std::string& message);
    static void global_debug(const std::string& context, const std::string& message);
    static void global_info(const std::string& context, const std::string& message);
    static void global_warn(const std::string& context, const std::string& message);
    static void global_error(const std::string& context, const std::string& message);

    RPObject(const std::string& name);

    const std::string& get_debug_name(void) const;
    void set_debug_name(const std::string& name);

    void trace(const std::string& message) const;
    void debug(const std::string& message) const;
    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;
    void fatal(const std::string& message) const;

protected:
    std::string debug_name_;
};

// ************************************************************************************************

inline const std::string& RPObject::get_debug_name(void) const
{
    return debug_name_;
}

inline void RPObject::set_debug_name(const std::string& name)
{
    debug_name_ = name;
}

inline void RPObject::trace(const std::string& message) const
{
    global_trace(debug_name_, message);
}

inline void RPObject::debug(const std::string& message) const
{
    global_debug(debug_name_, message);
}

inline void RPObject::info(const std::string& message) const
{
    global_info(debug_name_, message);
}

inline void RPObject::warn(const std::string& message) const
{
    global_warn(debug_name_, message);
}

inline void RPObject::error(const std::string& message) const
{
    global_error(debug_name_, message);
}

}
