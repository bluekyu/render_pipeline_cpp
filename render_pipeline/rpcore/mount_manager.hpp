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

#include <memory>

#include <boost/filesystem.hpp>

#include <render_pipeline/rpcore/rpobject.hpp>

class Filename;

namespace rpcore {

class RENDER_PIPELINE_DECL MountManager : public RPObject
{
public:
    /**
     * Convert virtual path to physical path.
     *
     * This function converts the path of Panda3D virutal file system to that of physical filesystem.
     * If failed, this will return empty string.
     */
    static boost::filesystem::path convert_to_physical_path(const Filename& path);

    MountManager();
    MountManager(const MountManager&) = delete;
    MountManager(MountManager&&) = delete;

    ~MountManager();

    MountManager& operator=(const MountManager&) = delete;
    MountManager& operator=(MountManager&&) = delete;

    const std::string& get_write_path() const;
    void set_write_path(const std::string& pth);

    const std::string& get_base_path() const;
    void set_base_path(const std::string& pth);

    const std::string& get_config_dir() const;
    void set_config_dir(const std::string& pth);

    bool get_do_cleanup() const;
    void set_do_cleanup(bool cleanup);

    bool get_lock();

    /** Returns whether the MountManager was already mounted by calling mount(). */
    bool is_mounted() const;

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
    void mount();

    /** Unmounts the VFS. */
    void unmount();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
