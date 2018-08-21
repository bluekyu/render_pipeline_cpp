# Render Pipeline C++ 빌드
**Languages**: [English](../build_rpcpp.md)

## 요구사항
명시된 **버전** 은 빌드 시스템에서 사용되는 것이며, 일치할 필요는 없다.

- 도구
  - CMake (빌드 도구)
  - Windows
    - [vcpkg](https://github.com/Microsoft/vcpkg) (선택사항이지만 권장): 7baf25786d2d4adb827ec4531d2fc2cb1fb0d5a6
- [(Patched) Panda3D](https://github.com/bluekyu/panda3d): master branch
- FreeType2: 2.5.2 (Panda3D third-party 포함된 버전)
- Boost: 1.65.0 이상
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [fmt](https://github.com/fmtlib/fmt)
- [spdlog](https://github.com/gabime/spdlog)
- Eigen3 (조건부): Panda3D 가 Eigen3 를 사용하지만 없을 경우
- Assimp (선택사항): rpassimp 플러그인 사용 시 필요



### CMake
Render Pipeline C++ 에서는 CMake 빌드 시스템을 사용한다.
CMake 를 사용하여 필요한 라이브러리들을 찾고, 빌드 도구(Unix Makefiles or Visual Studio solutions 등)를 생성한다.

CMake 에서 configure 를 하는 동안 라이브러리를 자동으로 찾는데, 일부 라이브러리 혹은 설치 경로가 다른 라이브러리의 경우
찾지 못할 수 있다. 이를 해결하려면, CMake configuration 창 혹은 Advanced 탭에서 라이브러리가 설치된 경로를 힌트로 주면 된다.

만일, `vcpkg` 를 사용한다면, 이 문제는 거의 없어진다.

### 외부 라이브러리 가이드
윈도우에서, 외부 라이브러리 사용 시 [vcpkg](https://github.com/Microsoft/vcpkg) 사용을 권장한다.
만일, `vcpkg` 를 사용하지 않는다면, 외부 라이브러리를 빌드 및 설치하고 CMake 캐시에 적절한 값을 설정하면 된다.

- Windows
  ```
  vcpkg install --triplet x64-windows boost-dll boost-any boost-multi-index spdlog yaml-cpp

  # conditional
  vcpkg install --triplet x64-windows eigen3

  # optional
  vcpkg install --triplet x64-windows assimp
  ```

- Ubuntu
  ```
  apt-get install libfmt-dev libspdlog-dev

  # optional
  apt-get install libassimp-dev
  ```


#### Panda3D
[Panda3d repository](https://github.com/bluekyu/panda3d) 에서 최신 빌드 파일을 받거나,
Panda3D 를 소스로부터 빌드하면 된다.

그런 다음, CMake 캐시에 `panda3d_ROOT` 를 설치 폴더로 설정한다.

#### Eigen3
Panda3D 에서 Eigen3 을 사용할 수 있고, 원래 Panda3D 에서는 Eigen3 을 사용할 경우 Panda3D 패키지에 기본적으로 설치한다.

그러나 [패치된 Panda3D](https://github.com/bluekyu/panda3d) 에서는 Eigen3 를 설치하지 않는다.
만일, Panda3D 에서 Eigen3 를 사용하고 이를 가지고 있지 않다면, Eigen3 를 설치해야 한다.

- Windows: `vcpkg install eigen3:x64-windows`
- Ubuntu: `apt-get install libeigen3-dev`

#### FreeType2
[Panda3d-Thirdparty repository](https://github.com/bluekyu/panda3d-thirdparty) 에서
최신 빌드 파일을 받거나,
[원래 panda3d-thirdparty repository](https://github.com/rdb/panda3d-thirdparty) 에서 빌드하면 된다.

그런 다음, FreeType2 에 대한 `FindPackage` 함수의 경우 configuration 에 힌트가 존재하지 않는다.
대신, 시스템 환경 변수에 `FREETYPE_DIR` 이름으로 힌트를 줄 수 있다.

따라서, 시스템 환경 변수를 사용하거나 CMake 에 해당 값을 설정하면 된다. (Integration with Plugin & Samples 참고)

#### Boost
- 필요한 라이브러리: algorithm, filesystem, dll, any, optional, variant, multi-index
- Windows
  - `vcpkg install --triplet x64-windows boost-dll boost-any boost-multi-index`
  - 또는, 공식 웹사이트로부터 Boost 라이브러리를 설치하고 `BOOST_ROOT` 를 설치 폴더로 설정한다.
    (자세한 것은, CMake 에서 FindBoost 사용법 참조)
- Ubuntu: `apt-get install libboost-filesystem-dev`

#### yaml-cpp
- Windows: `vcpkg install yaml-cpp:x64-windows`
- Ubuntu: github 저장소로부터 빌드

#### fmt
- Windows: `vcpkg install fmt:x64-windows`
- Ubuntu: `apt-get install libfmt-dev`

#### spdlog
- Windows: `vcpkg install spdlog:x64-windows`
- Ubuntu: `apt-get install libspdlog-dev`



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
```.cmake
cmake_minimum_required(VERSION 3.10.2)
project(render_pipeline_projects
    LANGUAGES NONE
)

# 개인 시스템에 맞는 경로 및 옵션을 사용
set(panda3d_ROOT "R:/usr/lib/panda3d" CACHE PATH "")
set(ENV{FREETYPE_DIR} "R:/usr/lib/panda3d-thirdparty/win-libs-vc14-x64/freetype")

# 선택사항
set(NvFlex_ROOT "R:/usr/lib/flex" CACHE PATH "")

# vcpkg 를 사용하지 않을 경우, 주석을 해제하고 경로 수정
#set(BOOST_ROOT "R:/usr/lib/boost" CACHE PATH "")
#set(spdlog_DIR "R:/usr/lib/spdlog/lib/cmake/spdlog" CACHE PATH "")
#set(yaml-cpp_DIR "R:/usr/lib/yaml-cpp/CMake" CACHE PATH "")
#set(OpenVR_ROOT "R:/usr/lib/openvr" CACHE PATH "")

add_subdirectory("render_pipeline_cpp")
add_subdirectory("rpcpp_plugins")
add_subdirectory("rpcpp_samples")
```

### 3. CMake 실행
`vcpkg` 를 사용한다면, CMake GUI 에서 **Specify toolchain file for cross-compiling** 을 선택하고,
vcpkg 에 설치된 toolchain 파일(vcpkg.cmake)을 선택.
명령줄에서는 `-DCMAKE_TOOLCHAIN_FILE=.....\vcpkg\scripts\buildsystems\vcpkg.cmake` 옵션을
사용해서 CMake 프로젝트 생성.

`vcpkg` 를 사용하지 않으며, 바로 CMake 실행.
