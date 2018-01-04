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



## 외부 동적 라이브러리
플러그인에서 외부 라이브러리를 사용할 수 있으나 동적 라이브러리를 사용할 때에는 동적 라이브러리를 로드하기 위한 작업이 필요하다.
만일, PluginManager 에서 플러그인을 로드할 때, 의존성이 있는 외부 동적 라이브러리 로드가 실패하면
플러그인 로드 실패 메시지를 내보내고, 해당 플러그인을 비활성화 한다.

외부 동적 라이브러리 로드는 일반적인 프로그램에서 필요한 동적 라이브러리를 검색하는 것과 동일한 과정을 거친다.
OS 마다 세세한 것은 다르지만, 현재 디렉토리나 PATH 환경 변수의 경로 등을 검색하여 필요한 동적 라이브러리가 있는지 찾고
로드를 수행한다. (일반 프로그램에서는 로드 실패 시에 못 찾은 파일에 대한 경고 메시지가 나타나지만,
플러그인 로드 시에는 그러한 메시지를 알 수 없다.)

### 동적 라이브러리 검색 경로 설정
외부 동적 라이브러리를 OS 라이브러리 로드 규칙에 맞게 설정 하면 된다.

간단하게는 외부 동적 라이브러리를 Working Directory 에 복사하는 방법이 있고, 외부 동적 라이브러리가 있는 경로를
PATH 환경 변수에 등록하는 방법도 있다.

그 외의 방법으로, Windows 는 [Dynamic-Link Library Search Order](https://msdn.microsoft.com/library/windows/desktop/ms682586(v=vs.85).aspx) 참조.

### 지연된 DLL 로드 사용 (Windows Only)
Windows 에서는 지연된 DLL 로드 방식을 사용하는 방법도 존재한다. 의존성이 있는 DLL 로드를 함수를 실행하는 시점까지
늦추는 방법이다. 이 방식을 사용하면 플러그인 안에서 필요한 외부 DLL을 직접 로드할 수 있다.

이러한 방법은 플러그인을 개발할 때 필요한 라이브러리를 같이 묶어서 배포 할 경우에 사용할 수 있다.
플러그인 폴더 안에 넣어서 같이 배포하게 되면, 외부 DLL 들의 경로가 고정되고 플러그인 코드 안에서 해당 경로의 DLL들을
직접 로드할 수 있게 된다.

Render Pipeline 플러그인(rpcpp_plugins)에서는 `windows_utils.cmake` 파일 안에 `windows_add_delay_load` 함수를 통해
CMake 에서 지연된 DLL 로드를 사용할 수 있도록 지원한다.

그리고 OpenVR 플러그인에서 이 방법을 사용하여 플러그인을 로드하므로,
해당 [CMake 파일](https://github.com/bluekyu/rpcpp_plugins/blob/master/openvr/CMakeLists.txt)
과 [소스 코드](https://github.com/bluekyu/rpcpp_plugins/blob/master/openvr/src/openvr_plugin.cpp)를 통해 예시를
확인할 수 있다.
