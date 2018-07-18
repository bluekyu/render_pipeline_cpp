# Logging
**Languages**: [English](../logging.md)

## Panda3D
[Panda3D Manual: Log Messages](https://www.panda3d.org/manual/index.php/Log_Messages) 참조.



## Render Pipeline
Render Pipeline 에서는 [spdlog](https://github.com/gabime/spdlog) 라이브러리를 사용해서 로그를 합니다.

로그 파일은 사용자 계정의 App Data 폴더 안에 `render_pipeline/logs` 폴더에 프로그램 이름으로 저장이 됩니다.
(만일, App Data 폴더가 없다면 Working Directory 에 저장됩니다.)
