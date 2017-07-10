# Render Pipeline C++ 빌드

## 요구사항
- Panda3D
- FreeType2 (Panda3D third-party 포함된 버전)
- Boost
- YAML-CPP
- spdlog
- flatbuffers
- CMake (빌드 도구)

### 선택사항
- Doxygen (Doxygen 문서 생성)



## CMake
Render Pipeline C++ 에서는 CMake 빌드 시스템을 사용한다.
CMake 를 사용하여 필요한 라이브러리들을 찾고, 빌드 도구(Unix Makefiles or Visual Studio solutions 등)를 생성한다.

CMake 에서 configure 를 하는 동안 라이브러리를 자동으로 찾는데, 일부 라이브러리 혹은 설치 경로가 다른 라이브러리의 경우
찾지 못할 수 있다.
이를 해결하려면, CMake configuration 창 혹은 Advanced 탭에서 라이브러리가 설치된 경로를 힌트로 주면 된다.

### FreeType2 찾기
FreeType2 에 대한 `FindPackage` 함수의 경우 configuration 에 힌트가 존재하지 않는다.
대신, 시스템 환경 변수에 `FREETYPE_DIR` 이름으로 힌트를 줄 수 있다.

### CMake 설정(configuration)
Configuration 창에서 `BUILD_SHARED_LIBS` 옵션은 Render Pipeline C++ 을 동적 라이브러리로 빌드할 지 여부를 설정한다.
`BUILD_ALL_SAMPLES` 옵션을 활성화하면 Render Pipeline C++ 의 모든 예제도 빌드한다.



## 빌드 구조
Render Pipeline C++ 는 핵심 라이브러리(render_pipeline), native 모듈, 내부 플러그인, 예제들을 빌드한다.
Native 모듈은 Python 버전의 Render Pipeline 과 동일하며 정적 링크된다.

내부 플러그인은 라이브러리가 아닌 플러그인 모듈로 빌드된다. 따라서 빌드 시에 동적 라이브러리 파일(.dll or .so files)만 생성하고,
`share/render_pipeline/rpplugins` 경로에 설치된다.



## Integration with Plugin & Samples
플러그인 프로젝트(https://github.com/bluekyu/rpcpp_plugins) 와
샘플 프로젝트(https://github.com/bluekyu/rpcpp_samples)가 있습니다.

플러그인과 샘플들을 함께 빌드하려면, 다음 방법을 사용하면 됩니다.

### 1. Directory Structure
```
- ROOT_DIR
  |- build                  # CMake 빌드 폴더
  |- render_pipeline_cpp    # Render Pipeline C++ 프로젝트 폴더
  |- rpcpp_plugins          # 플러그인 프로젝트 폴더
  |- rpcpp_samples          # 샘플 프로젝트 폴더
```

### 2. CMake
```
project(render_pipeline_projects)

# 개인 시스템에 맞는 경로을 사용
set(BOOST_ROOT "......" CACHE PATH "" FORCE)
set(Boost_USE_STATIC_LIBS true CACHE BOOL "" FORCE)
set(panda3d_ROOT "......" CACHE PATH "" FORCE)
set(ENV{FREETYPE_DIR} "......")
set(FlatBuffers_ROOT "......" CACHE PATH "" FORCE)
set(NvFlex_ROOT "......" CACHE PATH "" FORCE)
set(OpenVR_ROOT "......" CACHE PATH "" FORCE)
set(render_pipeline_PLUGIN_BUILD_OPENVR true CACHE BOOL "" FORCE)

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```
