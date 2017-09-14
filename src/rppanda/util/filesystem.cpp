/**
 * Render Pipeline C++
 *
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

#include <dcast.h>
#include <virtualFileSystem.h>
#include <virtualFileMountSystem.h>

#include "render_pipeline/rppanda/util/filesystem.hpp"

namespace rppanda {

// see VirtualFileSystem::do_get_file() in virtualFileSystem.cpp
boost::filesystem::path convert_path(const Filename& path)
{
    auto vfs = VirtualFileSystem::get_global_ptr();

    Filename pathname(path);
    pathname.make_absolute();

    Filename strpath = pathname.get_filename_index(0).get_fullpath().substr(1);
    strpath.set_type(path.get_type());

    Filename result = path;
    for (int k = 0, k_end = vfs->get_num_mounts(); k < k_end; ++k)
    {
        auto mount = vfs->get_mount(k);
        Filename mount_point = mount->get_mount_point();

        if (strpath == mount_point)
        {
            // Here's an exact match on the mount point.  This filename is the root
            // directory of this mount object.
            result = DCAST(VirtualFileMountSystem, mount)->get_physical_filename();
            break;
        }
        else if (strpath.length() > mount_point.length() &&
            mount_point == strpath.substr(0, mount_point.length()) &&
            strpath[mount_point.length()] == '/')
        {
            // This pathname falls within this mount system.
            Filename local_filename = strpath.substr(mount_point.length() + 1);
            result = DCAST(VirtualFileMountSystem, mount)->get_physical_filename() / local_filename;
            break;
        }
    }

#if WIN32
    return boost::filesystem::path(result.to_os_specific_w());
#else
    return boost::filesystem::path(result.to_os_specific());
#endif
}

}
