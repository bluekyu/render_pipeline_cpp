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

#include "render_pipeline/rpcore/rpobject.hpp"
#include "render_pipeline/rpcore/image.hpp"

namespace rpcore {

class RenderPipeline;
class GPUCommandList;
class RenderTarget;

/**
 * This class offers an interface to the gpu, allowing commands to be
 * pushed to a queue which then get executed on the gpu.
 */
class GPUCommandQueue: public RPObject
{
public:
    GPUCommandQueue(RenderPipeline* pipeline);
    ~GPUCommandQueue(void);

    GPUCommandList* get_command_list(void) const;

    size_t get_num_queued_commands(void) const;

    int get_num_processed_commands(void) const;

    /** Processes the n first commands of the queue. */
    void process_queue(void);

    /** Reloads the command shader. */
    void reload_shaders(void);

    /** Registers an new shader input to the command target. */
    void register_input(CPT_InternalName key, Texture* val);

private:
    /**
     * Registers all the command types as defines so they can be used
     * in a shader later on.
     */
    void register_defines(void);

    /** Creates the buffer used to transfer commands. */
    void create_data_storage(void);

    /** Creates the target which processes the commands. */
    void create_command_target(void);

    RenderPipeline* _pipeline;
    int _commands_per_frame = 1024;
    GPUCommandList* _command_list;
    PTA_int _pta_num_commands;
    RenderTarget* _command_target = nullptr;
    std::shared_ptr<Image> _data_texture = nullptr;
};

// ************************************************************************************************
inline GPUCommandList* GPUCommandQueue::get_command_list(void) const
{
    return _command_list;
}

inline int GPUCommandQueue::get_num_processed_commands(void) const
{
    return _pta_num_commands[0];
}

}
