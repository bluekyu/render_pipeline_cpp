# Render Pipeline C++ 사용
**Translation**: [English](../using_rpcpp.md)

## 요구사항
- Panda3D
- Boost

### Third-party 선택사항
내부 데이터에 접근하기 위해 헤더 파일을 사용할 경우 필요하다.

- YAML-CPP: YAML node 접근 시 필요.
- spdlog: 내부 spdlog logger 데이터에 접근 시 필요.



## 개발 환경 구성하기
### 빌드된 버전 사용하기
[Panda3D](https://github.com/bluekyu/panda3d) 와 [Render Pipeline C++](https://github.com/bluekyu/render_pipeline_cpp) 의
최신 빌드 버전은 저장소의 메인 페이지(README.md) 에서 다운로드 받을 수 있다.

또한, [플러그인](https://github.com/bluekyu/rpcpp_plugins) 과 [샘플](https://github.com/bluekyu/rpcpp_samples) 도
각 저장소에서 받을 수 있다.

### 빌드하기
[Render Pipeline C++ 빌드](build_rpcpp.md) 항목 참조.



## 폴더 구조 및 실행
최신 빌드 파일을 다운로드 받았다면, 아래와 같은 구조로 압축을 풀어준다.
소스로부터 빌드 했다면, 설치한 경로는 아래와 유사한 구조를 가진다.

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

위와 같은 구조를 가질 때, 샘플을 실행하기 위해서 DLL 혹은 so 파일의 경로 설정이 필요하다.

- Windows 에서는 `PATH` 환경 변수를 사용하거나, DLL을 실행 파일이 있는 폴더에 복사하는 방법을 이용할 수 있다.
(자세한 방법은 Windows DLL Search Path 검색)
- Linux 에서는 `LD_LIBRARY_PATH` 환경 변수를 사용할 수 있다.
(자세한 방법은 Linux Shared Library Search Path 검색)

또한, 위와 같은 구조는 한 예시이고, panda3d 폴더와 render_pipeline_cpp 폴더를 하나로 합쳐도 문제는 없다.
만일, 다른 식으로 폴더 구조를 구성한다면 base 폴더 및 config 폴더 위치만 `MountManager` 함수들을 사용하여
미리 설정해주면 된다. 자세한 방법은 [설정](configuration.md) 문서 참고.
