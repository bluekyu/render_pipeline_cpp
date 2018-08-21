# Panda3D 플러그인
**Languages**: [English](../../framework/panda3d_plugin.md)

## 플러그인 사용 방식
### PRC 설정
PRC 데이터나 파일에서 `load-file-type` 과 같은 옵션에 플러그인 이름을 추가하여 묵시적으로 로드할 수 있다.

예를 들어, `load-file-type` 의 경우 `Loader::load_file_types` 에서 로드를 수행하며,
"lib" prefix 와 ".so" 확장자를 자동으로 붙여주므로, 플러그인 이름만 추가해주면 된다.
(윈도우즈에서는 ".so" 확장자가 이후에 ".dll" 로 자동으로 변경된다.)

특히, `get_plugin_path()` 함수를 통해 플러그인의 경로를 탐색하는데, 이는 `plugin-path` 설정값을 따른다.
`plugin-path` 값은 기본적으로 "<auto>" 값을 가진다. 그리고 플러그인을 부르는 `load_dso` 함수에서는
"<auto>" 값만 존재할 경우, 자동으로 libp3dtool 파일이 있는 폴더를 검색한다.

이때, 시스템 환경 변수를 사용해서 경로를 지정할 수 있는데, PRC 에서 추가적으로 사용할 수 있는 환경 변수는 아래와 같다.
- `$MAIN_DIR`: 현재 작업 디렉토리
- `$THIS_RPC_DIR`: 현재 RPC 파일 폴더
- `$USER_APPDATA`: 사용자 APPDATA 폴더



### 명시적으로 로드
플러그인은 `load_dso` 함수를 사용하여 로드하면 된다.

```cpp
auto search_path = ...;     // ex) get_plugin_path().get_value()
void* tmp = load_dso(search_path, Filename::dso_filename("lib" + PLUGIN_NAME + ".so"));
if (tmp == nullptr) {
    // failed
}
```
