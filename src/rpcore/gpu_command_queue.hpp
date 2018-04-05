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

#include <pta_int.h>
#include <internalName.h>

#include <render_pipeline/rpcore/rpobject.hpp>

class Texture;

namespace rpcore {

class RenderPipeline;
class GPUCommandList;
class RenderTarget;
class Image;

/**
 * This class offers an interface to the gpu, allowing commands to be
 * pushed to a queue which then get executed on the gpu.
 */
class GPUCommandQueue : public RPObject
{
public:
    GPUCommandQueue(RenderPipeline& pipeline);
    ~GPUCommandQueue();

    GPUCommandList* get_command_list() const;

    size_t get_num_queued_commands() const;

    int get_num_processed_commands() const;

    /** Processes the n first commands of the queue. */
    void process_queue();

    /** Reloads the command shader. */
    void reload_shaders();

    /** Registers an new shader input to the command target. */
    void register_input(CPT_InternalName key, Texture* val);

private:
    /**
     * Registers all the command types as defines so they can be used
     * in a shader later on.
     */
    void register_defines();

    /** Creates the buffer used to transfer commands. */
    void create_data_storage();

    /** Creates the target which processes the commands. */
    void create_command_target();

    RenderPipeline& pipeline_;
    int commands_per_frame_ = 1024;
    std::unique_ptr<GPUCommandList> command_list_;
    PTA_int pta_num_commands_;
    std::unique_ptr<RenderTarget> command_target_;
    std::unique_ptr<Image> data_texture_;
};

// ************************************************************************************************
inline GPUCommandList* GPUCommandQueue::get_command_list() const
{
    return command_list_.get();
}

inline int GPUCommandQueue::get_num_processed_commands() const
{
    return pta_num_commands_[0];
}

}
