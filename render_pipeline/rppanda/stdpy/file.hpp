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

#pragma once

#include <virtualFileSystem.h>

namespace rppanda {

inline std::string join_to_string(const std::vector<Filename>& filenames)
{
    std::string resources;
    auto iter = filenames.begin();
    const auto iter_end = filenames.end();
    if (iter != iter_end)
    {
        resources += iter->c_str();
        ++iter;

        for (; iter != iter_end; ++iter)
        {
            resources += ", ";
            resources += iter->c_str();
        }
    }
    return resources;
}

inline Filename join(const Filename& lhs, const Filename& rhs)
{
    return lhs / rhs;
}

inline std::vector<std::string> listdir(const Filename& path)
{
    auto dir_list = VirtualFileSystem::get_global_ptr()->scan_directory(path);
    if (!dir_list)
        throw std::runtime_error(std::string("No such file or directory: ") + path.c_str());

    std::vector<std::string> files;
    for (size_t k=0, k_end=dir_list->get_num_files(); k < k_end; ++k)
        files.push_back(dir_list->get_file(k)->get_filename().get_basename());
    return files;
}

inline bool isfile(const Filename& path)
{
    return VirtualFileSystem::get_global_ptr()->is_regular_file(path);
}

inline bool isdir(const Filename& path)
{
    return VirtualFileSystem::get_global_ptr()->is_directory(path);
}

}
