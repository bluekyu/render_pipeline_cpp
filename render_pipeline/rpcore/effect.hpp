#pragma once

#include <map>
#include <memory>

#include <render_pipeline/rpcore/rpobject.hpp>

class Shader;

namespace rpcore {

/**
 * This class represents an instance of a compiled effect. It can be loaded
 * from a file.
 */
class RENDER_PIPELINE_DECL Effect: public RPObject
{
public:
    using OptionType = std::map<std::string, bool>;

public:
    static std::shared_ptr<Effect> load(const std::string& filename, const OptionType& options);

    Effect(void);
    ~Effect(void);

    int get_effect_id(void) const;

    bool get_option(const std::string& name) const;

    /** Sets the effect options, overriding the default options. */
    void set_options(const OptionType& options);

    bool do_load(const std::string& filename);

    /** Returns a handle to the compiled shader object for a given render pass. */
    Shader* get_shader_obj(const std::string& pass_id) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
