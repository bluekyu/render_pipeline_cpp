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

#include <render_pipeline/rpcore/rpobject.hpp>

class Filename;

namespace rpcore {

class RENDER_PIPELINE_DECL MountManager : public RPObject
{
public:
    MountManager();
    MountManager(const MountManager&) = delete;
    MountManager(MountManager&&) = delete;

    ~MountManager();

    MountManager& operator=(const MountManager&) = delete;
    MountManager& operator=(MountManager&&) = delete;

    const Filename& get_write_path() const;
    void set_write_path(const Filename& pth);

    const Filename& get_base_path() const;

    /**
     * Sets the path where the base shaders and models on are contained. This
     * is usually "share/render_pipeline" folder in install prefix.
     *
     * @param[in]   pth     Path of base directory. Relative path is based on
     *                      the current working directory.
     */
    void set_base_path(const Filename& pth);

    const Filename& get_config_dir() const;

    /**
     * Sets the path to the config directory. Usually this is the config/
     * directory located in base directory. However, if you want
     * to load your own configuration files, you can specify a custom config
     * directory here. Your configuration directory should contain the
     * pipeline.yaml, plugins.yaml, daytime.yaml and configuration.prc.
     *
     * It is highly recommended you use the pipeline provided config files, modify
     * them to your needs, and as soon as you think they are in a final version,
     * copy them over. Please also notice that you should keep your config files
     * up-to-date, e.g. when new configuration variables are added.
     *
     * Also, specifying a custom configuration_dir disables the functionality
     * of the PluginConfigurator and DayTime editor, since they operate on the
     * pipelines default config files.
     *
     * Set the directory to empty to use the default directory.
     *
     * @param[in]   pth     Path of config directory. Relative path is based on
     *                      the current working directory.
     */
    void set_config_dir(const Filename& pth);

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
