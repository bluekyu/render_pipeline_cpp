# Building Render Pipeline C++
**Translation**: [한국어](ko_kr/build_rpcpp.md)

## Requirements
- Panda3D
- FreeType2 (included in Panda3D third-party)
- Boost
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [spdlog](https://github.com/gabime/spdlog)
- [flatbuffers](https://github.com/google/flatbuffers)
- CMake (build tool)

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
Just build the repository with CMake, except install. CMake will find it automatically by 'User Package Registry'
of FindPackage.

#### spdlog
Build and install the repository with CMake. And set `spdlog_DIR` to installed directory.

#### flatbuffers
Build and install the repository with CMake. And set `FlatBuffers_ROOT` to installed directory.



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
─ ROOT_DIR
  ├ build                   # CMake build directory
  ├ render_pipeline_cpp     # Render Pipeline C++ project
  ├ rpcpp_plugins           # Plugin project
  ├ rpcpp_samples           # Sample project
  └ CMakeLists.txt          # CMake file (see below)
```

### 2. CMakeLists.txt File
```
project(render_pipeline_projects)

# write paths on your system
set(BOOST_ROOT "......" CACHE PATH "" FORCE)
set(Boost_USE_STATIC_LIBS true CACHE BOOL "" FORCE)
set(panda3d_ROOT "......" CACHE PATH "" FORCE)
set(ENV{FREETYPE_DIR} "......")
set(FlatBuffers_ROOT "......" CACHE PATH "" FORCE)

# optional paths
set(NvFlex_ROOT "......" CACHE PATH "" FORCE)
set(OpenVR_ROOT "......" CACHE PATH "" FORCE)

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```
