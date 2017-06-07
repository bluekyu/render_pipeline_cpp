# Building Render Pipeline C++  {#build_rpcpp}
**Translation**: @ref ko_kr_build_rpcpp "한국어"

## Requirements
* CMake (build tool)
* Boost (above 1.59)
* FreeType2
* Yaml-CPP
* Panda3D

### Optional
* OpenVR (to build OpenVR plugin)


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
