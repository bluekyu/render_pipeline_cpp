# Building Render Pipeline C++
**Translation**: [한국어](ko_kr/build_rpcpp.md)

## Requirements
The **Versions** are used by a build system and does **NOT** need to match.

- CMake (build tool): 3.8
- [(Patched) Panda3D](https://github.com/bluekyu/panda3d): develop branch
- FreeType2: 2.5.2 (included in Panda3D third-party)
- Boost: 1.64.0
- [yaml-cpp](https://github.com/jbeder/yaml-cpp): beb44b87 commit
- [spdlog](https://github.com/gabime/spdlog): 0.14.0
- [flatbuffers](https://github.com/google/flatbuffers): 0cf04ad commit

### Optional
- Doxygen (to create doxygen documents)



## CMake
Render Pipeline C++ uses CMake build system.
CMake will find required libraries and generate build tools (ex, Unix Makefiles or Visual Studio solutions.)

While configuring CMake, the first configuration may be failed because it cannot found some libraries.
To solve this problem, you can give hints for the path of the libraries in CMake configuration (or advanced tab).

### Third-party Guide
#### Panda3D
Install or build Panda3D library and set `panda3d_ROOT` to installed directory.

#### FreeType2
`FindPackage` function for FreeType2 does not have hint in configuration.
Instead, it uses `FREETYPE_DIR` in System Environment Variable as hint.

Therefore, use system environment variable OR set it in CMake (see Integration with Plugin & Samples)

#### Boost
Install Boost library and set `BOOST_ROOT` to installed directory (For details, see FindBoost usage in CMake)

#### YAML-CPP
Build and install the repository with CMake.
And set `yaml-cpp_DIR` to CMake directory in installed directory.

#### spdlog
Build and install the repository with CMake.
And set `spdlog_DIR` to CMake directory in installed directory.

#### flatbuffers
Build and install the repository with CMake.
And set `FlatBuffers_ROOT` to installed directory.



## Build Structure
Render Pipeline C++ will build core library (render_pipeline), native module, internal plugins.
Native module is the same as original Render Pipeline. This will be linked statically.

Internal plugins are built as Module, not library. Therefore, it creates only shared library (.dll or .so files.)
and install this into `share/render_pipeline/rpplugins`.



## Integration with Plugin & Samples
There are a plugin project (https://github.com/bluekyu/rpcpp_plugins) and
a sample project (https://github.com/bluekyu/rpcpp_samples) .

If you want to build with plugins and samples, you can do it using the following ways.

### 1. Directory Structure
```
─ PROJECT_DIR
  ├ build                   # CMake build directory
  ├ render_pipeline_cpp     # Render Pipeline C++ project
  ├ rpcpp_plugins           # Plugin project
  ├ rpcpp_samples           # Sample project
  └ CMakeLists.txt          # CMake file (see below)
```

### 2. CMakeLists.txt File
```
cmake_minimum_required(VERSION 3.8)
project(render_pipeline_projects)

# write paths and use options on your system
set(BOOST_ROOT "R:/usr/lib/boost" CACHE PATH "" FORCE)
#set(Boost_USE_STATIC_LIBS true CACHE BOOL "" FORCE)    # Use static library for boost
set(panda3d_ROOT "R:/usr/lib/panda3d" CACHE PATH "" FORCE)
set(ENV{FREETYPE_DIR} "R:/usr/lib/panda3d-thirdparty/win-libs-vc14-x64/freetype")
set(spdlog_DIR "R:/usr/lib/spdlog/lib/cmake/spdlog" CACHE PATH "" FORCE)
set(yaml-cpp_DIR "R:/usr/lib/yaml-cpp/CMake" CACHE PATH "" FORCE)
set(FlatBuffers_ROOT "R:/usr/lib/flatbuffers" CACHE PATH "" FORCE)

# optional paths
set(NvFlex_ROOT "R:/usr/lib/flex" CACHE PATH "" FORCE)
set(OpenVR_ROOT "R:/usr/lib/openvr" CACHE PATH "" FORCE)

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```
