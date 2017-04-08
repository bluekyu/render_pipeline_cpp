#pragma once

#include <memory>

#include <render_pipeline/rpcore/rpobject.h>

namespace rpcore {

class RenderPipeline;

class RPCPP_DECL MountManager : public RPObject
{
public:
    MountManager(RenderPipeline& pipeline);
    ~MountManager(void);

    const std::string& get_write_path(void) const;
    void set_write_path(const std::string& pth);

    const std::string& get_base_path(void) const;
    void set_base_path(const std::string& pth);

    const std::string& get_config_dir(void) const;
    void set_config_dir(const std::string& pth);

    bool get_do_cleanup(void) const;
    void set_do_cleanup(bool cleanup);

    bool get_lock(void);

    /** Returns whether the MountManager was already mounted by calling mount(). */
    bool is_mounted(void) const;

    /**
     * Inits the VFS Mounts. This creates the following virtual directory
     * structure, from which all files can be located:
     * /$$rp/  (Mounted from the render pipeline base directory)
     * + rpcore/
     * + shader/
     * + ...
     * /$rpconfig/ (Mounted from config/, may be set by user)
     * + pipeline.yaml
     * + ...
     * /$$rptemp/ (Either ramdisk or user specified)
     * + day_time_config
     * + shader_auto_config
     * + ...
     * /$$rpshader/ (Link to /$$rp/rpcore/shader)
     */
    void mount(void);

    /** Unmounts the VFS. */
    void unmount(void);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
