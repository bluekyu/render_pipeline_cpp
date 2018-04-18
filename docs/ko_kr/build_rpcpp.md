# Render Pipeline C++ 빌드
**Translation**: [English](../build_rpcpp.md)

## 요구사항
명시된 **버전** 은 빌드 시스템에서 사용되는 것이며, 일치할 필요는 없다.

- CMake (빌드 도구)
- [(Patched) Panda3D](https://github.com/bluekyu/panda3d): master branch
- FreeType2: 2.5.2 (Panda3D third-party 포함된 버전)
- Boost: 1.66.0
- [yaml-cpp](https://github.com/jbeder/yaml-cpp): 0.6.2
- [spdlog](https://github.com/gabime/spdlog): 0.16.3

### 선택사항
- Doxygen (Doxygen 문서 생성)



## CMake
Render Pipeline C++ 에서는 CMake 빌드 시스템을 사용한다.
CMake 를 사용하여 필요한 라이브러리들을 찾고, 빌드 도구(Unix Makefiles or Visual Studio solutions 등)를 생성한다.

CMake 에서 configure 를 하는 동안 라이브러리를 자동으로 찾는데, 일부 라이브러리 혹은 설치 경로가 다른 라이브러리의 경우
찾지 못할 수 있다. 이를 해결하려면, CMake configuration 창 혹은 Advanced 탭에서 라이브러리가 설치된 경로를 힌트로 주면 된다.

### 외부 라이브러리 가이드
#### Panda3D
Panda3D 라이브러리를 설치 혹은 빌드하고, `panda3d_ROOT` 를 설치 폴더로 설정한다.

#### FreeType2
FreeType2 에 대한 `FindPackage` 함수의 경우 configuration 에 힌트가 존재하지 않는다.
대신, 시스템 환경 변수에 `FREETYPE_DIR` 이름으로 힌트를 줄 수 있다.

따라서, 시스템 환경 변수를 사용하거나 CMake 에 해당 값을 설정하면 된다. (Integration with Plugin & Samples 참고)

#### Boost
Boost 라이브러리를 설치하고 `BOOST_ROOT` 를 설치 폴더로 설정한다. (자세한 것은, CMake 에서 FindBoost 사용법 참조)

#### YAML-CPP
라이브러리 저장소를 CMake 로 빌드 및 설치한다.
그리고 `yaml-cpp_DIR` 을 설치 폴더 안에 있는 CMake 폴더로 지정한다.

#### spdlog
라이브러리 저장소를 CMake 로 빌드 및 설치한다.
그리고 `spdlog_DIR` 을 설치 폴더 안에 있는 CMake 폴더로 지정한다.



## 빌드 구조
Render Pipeline C++ 는 핵심 라이브러리(render_pipeline), native 모듈, 내부 플러그인, 예제들을 빌드한다.
Native 모듈은 Python 버전의 Render Pipeline 과 동일하며 정적 링크된다.

내부 플러그인은 라이브러리가 아닌 플러그인 모듈로 빌드된다. 따라서 빌드 시에 동적 라이브러리 파일(.dll or .so files)만 생성하고,
`share/render_pipeline/rpplugins` 경로에 설치된다.



## 플러그인 및 샘플 통합 구성하기
플러그인 프로젝트(https://github.com/bluekyu/rpcpp_plugins) 와
샘플 프로젝트(https://github.com/bluekyu/rpcpp_samples) 가 있다.

플러그인과 샘플들을 함께 빌드하려면, 다음 방법을 사용하면 된다.

### 1. 폴더 구조
```
─ PROJECT_DIR
  ├ build                   # CMake 빌드 폴더
  ├ render_pipeline_cpp     # Render Pipeline C++ 프로젝트 폴더
  ├ rpcpp_plugins           # 플러그인 프로젝트 폴더
  ├ rpcpp_samples           # 샘플 프로젝드 폴더
  └ CMakeLists.txt          # CMake 파일 (아래 참조)
```

### 2. CMakeLists.txt 파일
```
cmake_minimum_required(VERSION 3.9)
project(render_pipeline_projects)

# 개인 시스템에 맞는 경로 및 옵션을 사용
set(BOOST_ROOT "R:/usr/lib/boost" CACHE PATH "" FORCE)
#set(Boost_USE_STATIC_LIBS true CACHE BOOL "" FORCE)    # Use static library for boost
set(panda3d_ROOT "R:/usr/lib/panda3d" CACHE PATH "" FORCE)
set(ENV{FREETYPE_DIR} "R:/usr/lib/panda3d-thirdparty/win-libs-vc14-x64/freetype")
set(spdlog_DIR "R:/usr/lib/spdlog/lib/cmake/spdlog" CACHE PATH "" FORCE)
set(yaml-cpp_DIR "R:/usr/lib/yaml-cpp/CMake" CACHE PATH "" FORCE)

# 선택사항
set(NvFlex_ROOT "R:/usr/lib/flex" CACHE PATH "" FORCE)
set(OpenVR_ROOT "R:/usr/lib/openvr" CACHE PATH "" FORCE)

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```
