/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
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

#pragma once

#include <texture.h>
#include <shader.h>
#include <shaderInput.h>

#include <unordered_map>
#include <vector>

#include <boost/variant.hpp>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpcore {

class RenderPipeline;
class RenderTarget;
class SimpleInputBlock;
class GroupedInputBlock;
class Image;

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
class RENDER_PIPELINE_DECL RenderStage : public RPObject
{
public:
    using RequireType = std::vector<std::string>;
    using ProduceType = std::vector<boost::variant<
        ShaderInput,
        std::shared_ptr<SimpleInputBlock>,
        std::shared_ptr<GroupedInputBlock>>>;
    using DefinesType = std::unordered_map<std::string, std::string>;

    static bool disabled_;

    RenderStage(RenderPipeline& pipeline, const std::string& stage_id);
    RenderStage(const RenderStage&) = delete;

    virtual ~RenderStage() {}

    RenderStage& operator=(const RenderStage&) = delete;

    virtual RequireType& get_required_inputs() const = 0;
    virtual RequireType& get_required_pipes() const = 0;

    virtual ProduceType get_produced_inputs() const;
    virtual ProduceType get_produced_pipes() const;

    virtual DefinesType get_produced_defines() const;

    virtual void create() = 0;
    virtual void update() {}
    virtual void reload_shaders() {}

    const std::string& get_stage_id() const;

    /**
     * This method sets a shader input on all stages, which is mainly used by the stage manager.
     */
    virtual void set_shader_input(const ShaderInput& inp);

    bool get_active() const;
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

    PT(Shader) load_shader(const std::vector<Filename>& args, bool stereo_post=false, bool use_post_gs=false) const;

    /**
     * Loads a shader from the plugin directory. This method is useful
     * for RenderStages created by plugins. For a description of the arguments,
     * see the load_shader function.
     */
    PT(Shader) load_plugin_shader(const std::vector<Filename>& args, bool stereo_post=false, bool use_post_gs=false) const;

    /**
     * Prepares the two textures required for processing invalid pixels
     * after executing the upscale pass.
     */
    std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>> prepare_upscaler(int max_invalid_pixels=2048) const;

    void handle_window_resize();
    virtual void set_dimensions() {}

protected:
    RenderPipeline& pipeline_;
    std::unordered_map<std::string, std::shared_ptr<RenderTarget>> targets_;

    virtual std::string get_plugin_id() const = 0;

private:
    PT(Shader) get_shader_handle(const Filename& path, const std::vector<Filename>& args, bool stereo_post, bool use_post_gs) const;

    const std::string stage_id_;
    bool active_ = true;
};

// ************************************************************************************************
inline const std::string& RenderStage::get_stage_id() const
{
    return stage_id_;
}

inline bool RenderStage::get_active() const
{
    return active_;
}

}
