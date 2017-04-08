#pragma once

#include <unordered_map>

#include <yaml-cpp/yaml.h>

namespace rplibs {

/** This method is a wrapper arround yaml_load, and provides error checking. */
bool load_yaml_file(const std::string& filename, YAML::Node& result);

/**
 * Behaves like load_yaml_file, but instead of creating nested dictionaries
 * it connects keys via '.'.
 */
using YamlFlatType = std::unordered_map<std::string, YAML::Node>;
YamlFlatType load_yaml_file_flat(const std::string& filename);

}	// namespace rplibs
