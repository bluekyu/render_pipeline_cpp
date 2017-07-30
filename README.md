# Render Pipeline C++

This project is C++ ported version of [Render Pipeline](https://github.com/tobspr/RenderPipeline).

And I am developing the project and new features for CR Software Framework (...).

- @subpage rendering
- @subpage render_pipeline_plugins
- @subpage render_pipeline_architecture
- @subpage document_translations



## Runtime Requirements
- Panda3D
- Boost

### Optional Third-party
- YAML-CPP: required when to access YAML node.
- spdlog: required when to access the internal logger of spdlog.



## Tested Platforms
I tested it in the following platforms:
- Intel CPU, NVIDIA GPU, Windows 10 64-bit, VS2013 / VS2015 / VS2017



## Build
See `docs/build_rpcpp.md` document.

### Current Third-party Version
- Panda3D: e715f0a2c96d336f5e2c25db47b967b46a88aaa9 (https://github.com/bluekyu/panda3d)
- FreeType2: 2.5.2 (in Panda3D)
- Boost: 1.64.0
- YAML-CPP: 0.5.3
- spdlog: 0.13.0
- flatbuffers: 1.7.1



## Related Projects
- Plugins for Render Pipeline C++: https://github.com/bluekyu/rpcpp_plugins
- Samples for Render Pipeline C++: https://github.com/bluekyu/rpcpp_samples
- Panda3D for CR Software Framework: https://github.com/bluekyu/panda3d



## License
See `LICENSE.md` file.
