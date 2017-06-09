# Render Pipeline C++

This project is to implement C++ version of [Render Pipeline](https://github.com/tobspr/RenderPipeline).

- @subpage build_rpcpp
- @subpage render_pipeline_plugins
- @subpage render_pipeline_architecture
- @subpage document_translations


## Build


### Integration with Plugin & Samples
```cmake
project(render_pipeline_package)

set(BOOST_ROOT "......" CACHE PATH "" FORCE)
set(Boost_USE_STATIC_LIBS true CACHE BOOL "" FORCE)
set(panda3d_ROOT "......" CACHE PATH "" FORCE)
set(ENV{FREETYPE_DIR} "......")
set(NvFlex_ROOT ""......")" CACHE PATH "" FORCE)
set(OpenVR_ROOT ""......")" CACHE PATH "" FORCE)
set(render_pipeline_PLUGIN_BUILD_OPENVR true CACHE BOOL "" FORCE)

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```
