#pragma once

#include <unordered_map>
#include <vector>

#include <texture.h>
#include <shader.h>
#include <shaderInput.h>

#include <render_pipeline/rpcore/rpobject.h>
#include <render_pipeline/rpcore/util/shader_input_blocks.h>
#include <render_pipeline/rpcore/image.h>

namespace rpcore {

class RenderPipeline;
class RenderTarget;
class SimpleInputBlock;
class GroupedInputBlock;

class StageData
{
public:
    enum class Type: int
    {
        SHADER_INPUT = 0,
        SIMPLE_INPUT_BLOCK,
        GROUP_INPUT_BLOCK
    };

public:
    StageData(const ShaderInput& inp): shader_input_(inp), _type(Type::SHADER_INPUT) {}
    StageData(const std::shared_ptr<SimpleInputBlock>& sib): _data(sib), _type(Type::SIMPLE_INPUT_BLOCK) {}
    StageData(const std::shared_ptr<GroupedInputBlock>& gib): _data(gib), _type(Type::GROUP_INPUT_BLOCK) {}

    bool is_type(Type type) const { return _type == type; }
    Type get_type(void) const { return _type; }

    const ShaderInput& get_shader_input(void) const { return shader_input_; }
    std::shared_ptr<SimpleInputBlock> get_simple_input_block(void) const { return std::static_pointer_cast<SimpleInputBlock>(_data); }
    std::shared_ptr<GroupedInputBlock> get_group_input_block(void) const { return std::static_pointer_cast<GroupedInputBlock>(_data); }

    std::string get_name(void) const
    {
        switch (_type)
        {
        case Type::SHADER_INPUT:
            return shader_input_.get_name()->get_name();
        case Type::SIMPLE_INPUT_BLOCK:
            return get_simple_input_block()->get_name();
        case Type::GROUP_INPUT_BLOCK:
            return get_group_input_block()->get_name();
        default:
            return "";
        }
    }

private:
    ShaderInput shader_input_;
    std::shared_ptr<void> _data;
    Type _type;
};

/**
 * This class is the abstract class for all stages used in the pipeline.
 * It represents a part of the pipeline render process. Each stage specifies
 * which pipes it uses and which pipes it produces. A pipe can be seen as a
 * texture, which gets modified. E.g. the gbuffer pass produces the gbuffer
 * pipe, the ambient occlusion pass produces the occlusion pipe and so on. The
 * lighting pass can then specify which pipes it needs and compute the image.
 * Using a pipe system ensures that new techniques can be inserted easily,
 * without the other techniques even being aware of them
 */
class RPCPP_DECL RenderStage: public RPObject
{
public:
    using RequireType = std::vector<std::string>;
    using ProduceType = std::vector<StageData>;
    using DefinesType = std::unordered_map<std::string, std::string>;

    static const size_t STEREO_VIEW_COUNT = 2;

    static bool disabled;

    RenderStage(RenderPipeline& pipeline, const std::string& stage_id);

    virtual ~RenderStage(void) {}

    virtual RequireType& get_required_inputs(void) const = 0;
    virtual RequireType& get_required_pipes(void) const = 0;

    virtual ProduceType get_produced_inputs(void) const;
    virtual ProduceType get_produced_pipes(void) const;

    virtual DefinesType get_produced_defines(void) const;

    virtual void create(void) = 0;
    virtual void update(void) {}
    virtual void reload_shaders(void) {}

    const std::string& get_stage_id(void) const;

    /**
     * This method sets a shader input on all stages, which is mainly used by the stage manager.
     */
    virtual void set_shader_input(const ShaderInput& inp);

    bool get_active(void) const;
    void set_active(bool state);

    /**
     * Create target and store it to RenderStage::targets.
     * @warning DO NOT delete the target.
     */
    std::shared_ptr<RenderTarget> create_target(const std::string& name);

    /**
     * Removes a previously registered target. This unregisters the
     * target, as well as removing it from the list of assigned targets.
     */
    void remove_target(const std::shared_ptr<RenderTarget>& target);

    PT(Shader) load_shader(const std::vector<std::string>& args, bool stereo_post=false, bool use_post_gs=false) const;

    /**
     * Loads a shader from the plugin directory. This method is useful
     * for RenderStages created by plugins. For a description of the arguments,
     * see the load_shader function.
     */
    PT(Shader) load_plugin_shader(const std::vector<std::string>& args, bool stereo_post=false, bool use_post_gs=false) const;

    /**
     * Prepares the two textures required for processing invalid pixels
     * after executing the upscale pass.
     */
    std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>> prepare_upscaler(int max_invalid_pixels=2048) const;

    void handle_window_resize(void);
    virtual void set_dimensions(void) {}

protected:
    RenderPipeline& pipeline_;
    std::unordered_map<std::string, std::shared_ptr<RenderTarget>> targets_;

    virtual std::string get_plugin_id(void) const = 0;

private:
    PT(Shader) get_shader_handle(const std::string& path, const std::vector<std::string>& args, bool stereo_post, bool use_post_gs) const;

    const std::string stage_id_;
    bool active_ = true;
};

// ************************************************************************************************
inline const std::string& RenderStage::get_stage_id(void) const
{
    return stage_id_;
}

inline bool RenderStage::get_active(void) const
{
    return active_;
}

}
