# Building Render Pipeline C++
**Languages**: [한국어](ko_kr/build_rpcpp.md)

## Requirements
The **Versions** are used by a build system and does **NOT** need to match. You can use another versions.

- CMake (build tool)
- [(Patched) Panda3D](https://github.com/bluekyu/panda3d): master branch
- FreeType2: 2.5.2 (included in Panda3D third-party)
- Boost: 1.66.0
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [fmt](https://github.com/fmtlib/fmt)
- [spdlog](https://github.com/gabime/spdlog)



## CMake
Render Pipeline C++ uses CMake build system.
CMake will find required libraries and generate build tools (ex, Unix Makefiles or Visual Studio solutions.)

While configuring CMake, the first configuration may be failed because it cannot find some libraries.
To solve this problem, you can give hints for the path of the libraries in CMake configuration (or advanced tab).

### Third-party Guide
In Windows, we recommend to use [vcpkg](https://github.com/Microsoft/vcpkg) for some third-party.
If you does not use `vcpkg`, then build and install those and setup proper values for CMake cache.


#### Panda3D
You can get the latest built files from [my Panda3d repository](https://github.com/bluekyu/panda3d) OR
build Panda3D from source.

And then set `panda3d_ROOT` to installed directory for CMake cache.

#### FreeType2
You can get the latest built files from
[my Panda3d-Thirdparty repository](https://github.com/bluekyu/panda3d-thirdparty) OR
build it from [original panda3d-thirdparty repository](https://github.com/rdb/panda3d-thirdparty).

And then, `FindPackage` function for FreeType2 does not have hint in configuration.
Instead, it uses `FREETYPE_DIR` in System Environment Variable as hint.

Therefore, use system environment variable OR set it in CMake (see Integration with Plugin & Samples)

#### Boost
- Windows: use vcpkg OR install Boost library from official website and
  set `BOOST_ROOT` to installed directory (For details, see FindBoost usage in CMake)
- Ubuntu: install Boost packages using apt

#### yaml-cpp
- Windows: `vcpkg install yaml-cpp:x64-windows`
- Ubuntu: build from github

#### fmt
- Windows: `vcpkg install fmt:x64-windows`
- Ubuntu: `apt-get install libfmt-dev`

#### spdlog
- Windows: `vcpkg install spdlog:x64-windows`
- Ubuntu: `apt-get install libspdlog-dev`



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
```.cmake
cmake_minimum_required(VERSION 3.9)
project(render_pipeline_projects
    LANGUAGES NONE
)

# write paths and use options on your system
set(BOOST_ROOT "R:/usr/lib/boost" CACHE PATH "" FORCE)
#set(Boost_USE_STATIC_LIBS true CACHE BOOL "" FORCE)    # Use static library for boost
set(panda3d_ROOT "R:/usr/lib/panda3d" CACHE PATH "" FORCE)
set(ENV{FREETYPE_DIR} "R:/usr/lib/panda3d-thirdparty/win-libs-vc14-x64/freetype")

# if you use custom build, uncomment and write paths.
#set(spdlog_DIR "R:/usr/lib/spdlog/lib/cmake/spdlog" CACHE PATH "" FORCE)
#set(yaml-cpp_DIR "R:/usr/lib/yaml-cpp/CMake" CACHE PATH "" FORCE)

# optional paths
set(NvFlex_ROOT "R:/usr/lib/flex" CACHE PATH "" FORCE)
set(OpenVR_ROOT "R:/usr/lib/openvr" CACHE PATH "" FORCE)

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```

### 3. Run CMake
If `vcpkg` is used, select "Specify toolchain file for cross-compiling" and
choose toolchain file (vcpkg.cmake) of vcpkg in CMake GUI.
In command line, generate CMake project with
`-DCMAKE_TOOLCHAIN_FILE=.....\vcpkg\scripts\buildsystems\vcpkg.cmake` option.

If `vcpkg` is not used, just run CMake.
