# Logging
**Languages**: [한국어](ko_kr/logging.md)

## Panda3D
See [Panda3D Manual: Log Messages](https://www.panda3d.org/manual/index.php/Log_Messages).



## Render Pipeline
In Render Pipeline, [spdlog](https://github.com/gabime/spdlog) library is used for logging.

Log file is saved with program name in `render_pipeline/logs` directory in User App Data directory.
(If App Data directory does not exist, the file will be saved on current working directory.)
