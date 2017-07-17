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
