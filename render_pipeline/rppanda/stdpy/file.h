#pragma once

#include <virtualFileSystem.h>

namespace rppanda {

inline std::string join(const std::string& lhs, const Filename& rhs)
{
	return std::string(Filename::from_os_specific(lhs) / rhs);
}

inline std::vector<std::string> listdir(const std::string& path)
{
	auto dir_list = VirtualFileSystem::get_global_ptr()->scan_directory(Filename::from_os_specific(path));
	if (!dir_list)
		throw std::runtime_error("No such file or directory: " + path);

	std::vector<std::string> files;
	for (size_t k=0, k_end=dir_list->get_num_files(); k < k_end; ++k)
		files.push_back(dir_list->get_file(k)->get_filename().get_basename());
	return files;
}

inline bool isfile(const std::string& path)
{
	return VirtualFileSystem::get_global_ptr()->is_regular_file(Filename::from_os_specific(path));
}

inline bool isdir(const std::string& path)
{
	return VirtualFileSystem::get_global_ptr()->is_directory(Filename::from_os_specific(path));
}

}
