# Render Pipeline C++

This project is C++ ported version of [Render Pipeline](https://github.com/tobspr/RenderPipeline).

And I am developing the project and new features for CR Software Framework (...).

#### Build Status
| Build Status                 | About                      |
| :--------------------------: | :------------------------: |
| [![azure-rpcpp]][azure-link] | Visual Studio 2017         |
| [![ci-badge]][ci-link]       | Visual Studio 2017 Preview |

[ci-badge]: https://ci.appveyor.com/api/projects/status/uo5j9rd751aux6l1/branch/master?svg=true "AppVeyor build status"
[ci-link]: https://ci.appveyor.com/project/bluekyu/render-pipeline-cpp/branch/master "AppVeyor build link"
[azure-rpcpp]: https://dev.azure.com/bluekyu/rpcpp-devops/_apis/build/status/render_pipeline_cpp/render_pipeline_cpp "Azure Pipelines status"
[azure-link]: https://dev.azure.com/bluekyu/rpcpp-devops/_build/latest?definitionId=4 "Azure Pipelines link"

You can download built files from each Build Page.

**Note**: These builds are built with *patched* [Panda3D](https://github.com/bluekyu/panda3d).



## Documents
See [docs/index.md](docs/index.md) document.



## Library Requirements
- Panda3D
- Boost

### Optional Third-party
These are required when you include related headers to access internal data.

- [yaml-cpp](https://github.com/jbeder/yaml-cpp): required when to access YAML node.
- [spdlog](https://github.com/gabime/spdlog): required when to access the internal logger of spdlog.
- [assimp](https://github.com/assimp/assimp): required to use rpassimp plugin.



## Tested Platforms
I tested it in the following platforms:
- Intel CPU, NVIDIA GPU, Windows 10 64-bit, Visual Studio 2017
- Intel CPU, Intel GPU (UHD Graphics 620), Windows 10 64-bit, Visual Studio 2017



## Build
See [docs/build_rpcpp.md](docs/build_rpcpp.md) document.



## Related Projects
- Panda3D Third-party: https://github.com/bluekyu/panda3d-thirdparty
- (patched) Panda3D: https://github.com/bluekyu/panda3d
- Plugins for Render Pipeline C++: https://github.com/bluekyu/rpcpp_plugins
- Samples and snippets for Render Pipeline C++: https://github.com/bluekyu/rpcpp_samples



## License
See `LICENSE.md` file.

And license of original (python based) [Render Pipeline](https://github.com/tobspr/RenderPipeline)
is `LICENSE-RenderPipeline.md` file in `thirdparty-licenses` directory.

### Third-party Licenses
See `thirdparty-licenses` directory.
