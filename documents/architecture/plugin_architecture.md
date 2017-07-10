# Plugin Architecture  {#plugin_architecture}
**Internalization**: @subpage ko_kr_plugin_architecture "한국어"

Plugins of Render Pipeline are installed in `etc/render_pipeline/rpplugins` and
Render Pipeline will load plugins enabled in `etc/render_pipeline/plugins.yaml` configuration.

Internal plugins are installed the directory, automatically.
If you create custom plugin, just copy the plugin directory into `rpplugins` directory.

## Structure
A plugin directory should have two files, `plguin.dll` and `config.yaml`.

plugin.dll is compiled shared library and config.yaml describes your plguin configuration.
If you have no plugin configuration, just create empty configuration file.
