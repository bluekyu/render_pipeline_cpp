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

#include "rplibs/yaml.hpp"

#include <chrono>
#include <iostream>

#include <filename.h>
#include <virtualFileSystem.h>

namespace rplibs {

/** Internal method to flatten a dictionary. */
static void flatten(YamlFlatType& root, const YAML::Node& node, const std::string& parent_key="")
{
    if (node.IsMap())
    {
        for (auto iter=node.begin(), iter_end=node.end(); iter != iter_end; ++iter)
        {
            flatten(root, iter->second, parent_key + iter->first.as<std::string>() + ".");
        }
    }
    else
    {
        std::string key = parent_key;
        key.pop_back();        // remove the last dot.
        root[std::move(key)] = node;
    }
}

bool load_yaml_file(const std::string& filename, YAML::Node& result)
{
    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();

    const std::chrono::system_clock::time_point& start = std::chrono::system_clock::now();

    std::istream* file = nullptr;
    try
    {
        file = vfs->open_read_file(filename, false);
        if (!file)
            throw std::runtime_error("Failed: vfs->open_read_file(filename, false)");
    }
    catch (const std::exception& err)
    {
        std::cout << "Failed to read file (" << filename << "): " << err.what();
        return false;
    }

    try
    {
        result = YAML::Load(*file);
    }
    catch (const std::exception& err)
    {
        std::cout << "Failed to parse YAML file (" << filename << "): " << err.what();
        vfs->close_read_file(file);
        return false;
    }

    vfs->close_read_file(file);

    float duration = (std::chrono::system_clock::now() - start).count();

    // Optionally print out profiling information
    //std::cout << "Took " << duration << " ms to load " << filename << std::endl;

    return true;
}

YamlFlatType load_yaml_file_flat(const std::string& filename)
{
    YamlFlatType root;

    YAML::Node node;
    if (load_yaml_file(filename, node))
        flatten(root, node);

    return root;
}

}
