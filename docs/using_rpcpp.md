# Using Render Pipeline C++
**Translation**: [한국어](ko_kr/using_rpcpp.md)

## Requirements
- Panda3D
- Boost

### Optional Third-party
These are required when you include related headers to access internal data.

- YAML-CPP: required when to access YAML node.
- spdlog: required when to access the internal logger of spdlog.



## Development Environment Setup
### Using Pre-built
You can download the latest built versions in main page (README.md) of each repository,
[Panda3D](https://github.com/bluekyu/panda3d) and [Render Pipeline C++](https://github.com/bluekyu/render_pipeline_cpp).

Also, [plugins](https://github.com/bluekyu/rpcpp_plugins) and [samples](https://github.com/bluekyu/rpcpp_samples) can be
downloaded from each repository.

### Building from Sources
See [Building Render Pipeline C++](build_rpcpp.md) page.



## Directory Structure and Run
If you download the latest built files, extract the files as the below.
If you build it from sources, the install directory has a structure as the below.

```
- panda3d               # (from panda3d)
  ├ bin
  ├ etc
  └ ...

- render_pipeline_cpp   # (from render_pipeline_cpp)
  ├ bin                 # Render Pipeline shared library and samples (from rpcpp_samples)
  ├ etc                 # Configuration files of samples (from rpcpp_samples)
  ├ include
  ├ lib
  └ share
    └ render_pipeline
      ├ ...
      └ rpplugins       # (from render_pipeline_cpp and rpcpp_plugins)
```
