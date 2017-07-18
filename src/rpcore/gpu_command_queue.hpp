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
