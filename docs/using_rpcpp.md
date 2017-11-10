# Using Render Pipeline C++
**Translation**: [한국어](ko_kr/using_rpcpp.md)

## Requirements
- Panda3D
- Boost

### Optional Third-party
These are required when you include related headers to access internal data.

- YAML-CPP: required when to access YAML node.
- spdlog: required when to access the internal logger of spdlog.



## Directory Structure and Run





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
