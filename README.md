# Render Pipeline C++

[![Windows Build status](https://ci.appveyor.com/api/projects/status/j6q88qp5sxskog5p/branch/master?svg=true)](https://ci.appveyor.com/project/bluekyu/render-pipeline-cpp/branch/master)

This project is C++ ported version of [Render Pipeline](https://github.com/tobspr/RenderPipeline).

And I am developing the project and new features for CR Software Framework (...).


## Documents
- See `docs/index.md` document.



## Library Requirements
- Panda3D
- Boost

### Optional Third-party
These are required when you include related headers to access internal data.

- YAML-CPP: required when to access YAML node.
- spdlog: required when to access the internal logger of spdlog.



## Tested Platforms
I tested it in the following platforms:
- Intel CPU, NVIDIA GPU, Windows 10 64-bit, VS2013 / VS2017



## Build
See `docs/build_rpcpp.md` document.

### Current Third-party Version
- Panda3D: https://github.com/bluekyu/panda3d/tree/develop
- FreeType2: 2.5.2 (in Panda3D)
- Boost: 1.64.0
- YAML-CPP: 0.5.3
- spdlog: 0.13.0
- flatbuffers: 1.7.1



## Related Projects
- Plugins for Render Pipeline C++: https://github.com/bluekyu/rpcpp_plugins
- Samples for Render Pipeline C++: https://github.com/bluekyu/rpcpp_samples



## License
See `LICENSE.md` file.

### Third-party Licenses
See `thridparty-licenses.md` file.
