# Building Render Pipeline C++
**Translation**: [한국어](ko_kr/build_rpcpp)

## Requirements
- Panda3D
- FreeType2 (included in Panda3D third-party)
- Boost
- YAML-CPP
- spdlog
- flatbuffers
- CMake (build tool)

### Optional
- OpenVR (to build OpenVR plugin)
- Doxygen (to create doxygen documents)



## CMake
Render Pipeline C++ has cmake build system.
CMake will find required libraries and generate build tools (ex, Unix Makefiles or Visual Studio solutions.)

While configuring CMake, the first configuration may be failed because it cannot found some libraries.
To solve this problem, you can give hints for the path of the libraries in CMake configuration (or advanced tab).

### Finding FreeType2
`FindPackage` function for FreeType2 does not have hint in configuration.
Instead, the hint can be configured in System Environment Variable as `FREETYPE_DIR`.

### CMake Configuration
In configuration, `BUILD_SHARED_LIBS` option sets buliding shared libraries for Render Pipeline C++.
Enabling `BUILD_ALL_SAMPLES` option is to build all examples in Render Pipeline C++.



## Build Structure
Render Pipeline C++ will build core library (render_pipeline), native module, internal plugins and samples.
Native module is the same as original Render Pipeline. This will be linked statically.

Internal plugins are built as Module, not library. Therefore, it creates only shared library (.dll or .so files.)
and install this into `etc/render_pipeline/rpplugins`.

If the `BUILD_ALL_SAMPLES` option is enabled, all samples are built and installed in `bin` directory.
And enabling `BUILD_DOCUMENTS` option will build documents using Doxygen.



## Integration with Plugin & Samples
```cmake
project(render_pipeline_package)

set(BOOST_ROOT "......" CACHE PATH "" FORCE)
set(Boost_USE_STATIC_LIBS true CACHE BOOL "" FORCE)
set(panda3d_ROOT "......" CACHE PATH "" FORCE)
set(ENV{FREETYPE_DIR} "......")
set(FlatBuffers_ROOT "......" CACHE PATH "" FORCE)
set(NvFlex_ROOT "......" CACHE PATH "" FORCE)
set(OpenVR_ROOT "......" CACHE PATH "" FORCE)
set(render_pipeline_PLUGIN_BUILD_OPENVR true CACHE BOOL "" FORCE)

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```
