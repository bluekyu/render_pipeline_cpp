# Plugin System
**Translation**: [한국어](ko_kr/plugin_system)

## Overview
Plugins of Render Pipeline are installed in `etc/render_pipeline/rpplugins` and
Render Pipeline will load plugins enabled in `etc/render_pipeline/plugins.yaml` configuration.

Internal plugins are installed into the directory, automatically.
If you create custom a plugin, just copy the plugin directory into `rpplugins` directory.



## Structure
A plugin directory should have two files, DLL (or so) file and `config.yaml` file.

DLL file is a compiled shared library and `config.yaml` describes your plguin configuration.
If you have no plugin configuration, just create empty configuration file.
