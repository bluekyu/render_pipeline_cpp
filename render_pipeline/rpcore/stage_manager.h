#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <render_pipeline/rpcore/rpobject.h>

class ShaderInput;

namespace rpcore {

class RenderPipeline;
class RenderStage;
class SimpleInputBlock;
class GroupedInputBlock;

/**
 * This manager takes a list of RenderStages and puts them into an order,
 * while connecting the different pipes, inputs, ubos and defines.
 */
class RENDER_PIPELINE_DECL StageManager: public RPObject
{
public:
    using DefinesType = std::unordered_map<std::string, std::string>;

    /** Constructs the stage manager. */
    StageManager(RenderPipeline& pipeline);

    ~StageManager(void);

    DefinesType& get_defines(void);
    const DefinesType& get_defines(void) const;

    void add_input(const ShaderInput& inp);

    const ShaderInput& get_pipe(const std::string& pipe_name) const;

    /** Add a new stage. */
    void add_stage(const std::shared_ptr<RenderStage>& stage);

    const std::vector<std::shared_ptr<RenderStage>>& get_stages(void) const;

    /** Returns a handle to an instantiated stage. */
    std::shared_ptr<RenderStage> get_stage(const std::string& stage_id) const;

    void add_input_blocks(const std::shared_ptr<SimpleInputBlock>& input_block);
    void add_input_blocks(const std::shared_ptr<GroupedInputBlock>& input_block);

    /** Setups the stages. */
    void setup(void);

    /**
     * This pass sets the shaders to all passes and also generates the shader configuration.
     */
    void reload_shaders(void);

    /**
     * Calls the update method for each registered stage. Inactive stages are skipped.
     */
    void update(void);

    /**
     * Method to get called when the window got resized.
     * Propagates the resize event to all registered stages.
     */
    void handle_window_resize(void);

    /**
     * Writes the shader auto config, based on the defines specified by the
     * different stages.
     */
    void write_autoconfig(void);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
