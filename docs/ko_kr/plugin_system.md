# 플러그인 시스템
**Translation**: [English](../plugin_system.md)

## 개요
Render Pipeline C++ 플러그인들은 `etc/render_pipeline/rpplugins` 경로에 설치되고,
Render Pipeline 실행 시에 `etc/render_pipeline/plugins.yaml` 설정에서 활성화된 플러그인을 로드한다.

내부 플러그인들은 자동으로 해당 경로에 설치된다.
Custom 플러그인을 만들 경우에는 `rpplugins` 폴더에 플러그인 폴더를 복사하면 된다.



## 구조
플러그인 폴더는 파일 DLL (혹은 so) 파일과 `config.yaml` 파일을 반드시 가져야 한다.

DLL 파일은 컴파일된 동적 라이브러리이며, `config.yaml` 은 플러그인 설정을 포함한다.
플러그인 설정이 없다면, 빈 설정 파일을 만들면 된다.
