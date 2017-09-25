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

#include "render_pipeline/rpcore/mount_manager.hpp"

#include <dcast.h>
#include <filename.h>
#include <virtualFileSystem.h>
#include <virtualFileMountRamdisk.h>
#include <virtualFileMountSystem.h>
#include <config_util.h>

#include <boost/filesystem.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rppanda/stdpy/file.hpp"
#include "render_pipeline/rppanda/util/filesystem.hpp"

namespace rpcore {

class MountManager::Impl
{
public:
    void set_write_path(const Filename& pth);

    bool get_lock();

    void mount(MountManager& self);

    Filename find_basepath() const;

    void wrtie_lock();

    /**
     * @param[in] fname    Panda3D path (unix-style).
     */
    bool try_remove(MountManager& self, const std::string& fname);

    /** Gets called when the manager is destructed. */
    void on_exit_cleanup(MountManager& self);

public:
    /** This is Panda3D path (unix-style). */
    ///@{
    Filename base_path_;
    Filename lock_file_ = "instance.pid";
    Filename write_path_;
    Filename config_dir_;
    ///@}

    bool mounted_ = false;
    bool do_cleanup_ = true;
};

void MountManager::Impl::set_write_path(const Filename& pth)
{
    if (pth.empty())
    {
        write_path_ = "";
        lock_file_ = "instance.pid";
    }
    else
    {
        write_path_ = pth;
        write_path_.make_absolute();
        lock_file_ = rppanda::join(write_path_, "instance.pid");
    }
}

bool MountManager::Impl::get_lock()
{
    // Check if there is a lockfile at all
    if (rppanda::isfile(lock_file_))
    {
        // Read process id from lockfile
        // TODO: implement this.
        return true;
    }
    else
    {
        wrtie_lock();
        return true;
    }
}

void MountManager::Impl::mount(MountManager& self)
{
    self.debug("Setting up virtual filesystem");
    mounted_ = true;

    auto convert_path = [](Filename pth) {
        pth.make_absolute();
        return pth;
    };
    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();

    // Mount config dir as $$rpconf
    if (config_dir_.empty())
    {
        const Filename& config_dir = convert_path(rppanda::join(base_path_, "config/"));
        self.debug(fmt::format("Mounting auto-detected config dir: {}", config_dir.to_os_specific()));
        vfs->mount(config_dir, "/$$rpconfig", 0);
    }
    else
    {
        self.debug(fmt::format("Mounting custom config dir: {}", config_dir_.to_os_specific()));
        vfs->mount(convert_path(config_dir_), "/$$rpconfig", 0);
    }

    // Mount directory structure
    vfs->mount(convert_path(base_path_), "/$$rp", 0);
    vfs->mount(convert_path(rppanda::join(base_path_, "rpcore/shader")), "/$$rp/shader", 0);

    // Mount the pipeline temp path:
    // If no write path is specified, use a virtual ramdisk
    if (write_path_.empty())
    {
        self.debug("Mounting ramdisk as /$$rptemp");
        vfs->mount(new VirtualFileMountRamdisk, "/$$rptemp", 0);
    }
    else
    {
        // In case an actual write path is specified:
        // Ensure the pipeline write path exists, and if not, create it
        if (!rppanda::isdir(write_path_))
        {
            self.debug("Creating temporary path, since it does not exist yet");
            try
            {
                vfs->make_directory_full(write_path_);
            }
            catch (const std::exception& err)
            {
                self.fatal(std::string("Failed to create temporary path: ") + err.what());
            }
        }

        self.debug(fmt::format("Mounting {} as /$$rptemp", write_path_.to_os_specific()));
        vfs->mount(convert_path(write_path_), "/$$rptemp", 0);
    }

    auto& model_path = get_model_path();
    model_path.prepend_directory("/$$rp");
    model_path.prepend_directory("/$$rp/shader");
    model_path.prepend_directory("/$$rptemp");
}

Filename MountManager::Impl::find_basepath() const
{
    // NOTE: Render Pipeline C++ will install resources directory into "share/render_pipeline"
    Filename pth = rppanda::join(rppanda::convert_path(
        boost::dll::this_line_location().parent_path()),
        "../share/render_pipeline");
    pth.make_absolute();
    return pth;
}

void MountManager::Impl::wrtie_lock()
{
    // TODO: implmeent this.
}

bool MountManager::Impl::try_remove(MountManager& self, const std::string& fname)
{
    try
    {
        self.debug("Try to remove '" + fname + "'");
        boost::filesystem::remove(Filename(fname).to_os_specific());
        return true;
    }
    catch(...)
    {
    }
    return false;
}

void MountManager::Impl::on_exit_cleanup(MountManager& self)
{
    if (do_cleanup_)
    {
        self.debug("Cleaning up ..");

        if (!write_path_.empty())
        {
            // Try removing the lockfile
            // TODO: uncomment
            //try_remove(lock_file_);

            // Check for further tempfiles in the write path
            // We explicitely use os.listdir here instead of panda's listdir,
            // to work with actual paths.
            VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
            const auto& write_path_os = rppanda::convert_path(write_path_);
            for (const auto& fpath: boost::filesystem::directory_iterator(write_path_os))
            {
                const std::string& fname = fpath.path().filename().generic_string();
                const std::string& pth = fpath.path().generic_string();

                // Tempfiles from the pipeline start with "$$" to distinguish
                // them from user created files.
                if (rppanda::isfile(pth) && fname.substr(0, 2) == "$$")
                    try_remove(self, pth);
            }

            // Delete the write path if no files are left.
            if (std::count_if(
                boost::filesystem::directory_iterator(write_path_os),
                boost::filesystem::directory_iterator(),
                [](const boost::filesystem::directory_entry&){return true; }) < 1)
            {
                try
                {
                    boost::filesystem::remove(write_path_os);
                    self.debug(fmt::format("Remove '{}'", rppanda::convert_path(write_path_os).to_os_specific()));
                }
                catch (...)
                {
                }
            }
        }
    }
}

// ************************************************************************************************

MountManager::MountManager(): RPObject("MountManager"), impl_(std::make_unique<Impl>())
{
    set_base_path(impl_->find_basepath());

    debug(fmt::format("Auto-Detected base path to {}", impl_->base_path_.to_os_specific()));
}

MountManager::~MountManager()
{
    impl_->on_exit_cleanup(*this);
}

void MountManager::mount() { impl_->mount(*this); }
void MountManager::set_write_path(const Filename& pth) { impl_->set_write_path(pth); }
bool MountManager::get_lock() { return impl_->get_lock(); }

inline const Filename& MountManager::get_write_path() const
{
    return impl_->write_path_;
}

inline const Filename& MountManager::get_base_path() const
{
    return impl_->base_path_;
}

void MountManager::set_base_path(const Filename& pth)
{
    debug(fmt::format("Set base path to '{}'", pth.to_os_specific()));
    impl_->base_path_ = pth;
    impl_->base_path_.make_absolute();
}

inline const Filename& MountManager::get_config_dir() const
{
    return impl_->config_dir_;
}

void MountManager::set_config_dir(const Filename& pth)
{
    impl_->config_dir_ = pth.get_fullpath();
}

inline bool MountManager::get_do_cleanup() const
{
    return impl_->do_cleanup_;
}

inline void MountManager::set_do_cleanup(bool cleanup)
{
    impl_->do_cleanup_ = cleanup;
}

inline bool MountManager::is_mounted() const
{
    return impl_->mounted_;
}

void MountManager::unmount()
{
    throw std::runtime_error("TODO");
}

}
