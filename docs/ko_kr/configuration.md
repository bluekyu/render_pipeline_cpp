# 설정
**Translation**: [English](../configuration.md)

## Panda3D PRC 설정

Panda3D 에서는 PRC 파일들을 사용해서 라이브러리의 설정을 조절할 수 있다. 자세한 내용은 아래 문서를 참조.
- https://github.com/panda3d/panda3d/blob/master/panda/src/doc/howto.use_config.txt
- https://www.panda3d.org/manual/index.php/Accessing_Config_Vars_in_a_Program

### 요약
컴파일 과정에서 따로 설정을 하지 않는다면, 런타임 시에 파일을 로드하거나 암시적으로 파일을 로드하게 된다.

#### 암시적 설정
환경 변수 설정이 없을 경우, 기본 검색 경로는 `<auto>etc` 폴더이고, `<auto>` 위치는
`libp3dtool` 동적 라이브러리 파일을 기준으로 부모 디렉토리를 차례대로 검색한다.

예를 들면, 아래와 같이 검색하게 된다.
```
./libp3dtool.dll/../etc
./libp3dtool.dll/../../etc
./libp3dtool.dll/../../../etc
......
```

이 경우가 실패하게 되면, 현재 실행 파일을 기준으로 위와 동일하게 검색한다.

#### 런타임 설정
`load_prc_file` 함수를 사용하여 PRC 파일 로드할 수 있다.

### Common Issues
```
Warning: unable to auto-locate config files in directory named by "<auto>etc".
Known pipe types:
(all display modules loaded.)
No graphics pipe is available!
Your Config.prc file must name at least one valid panda display
library via load-display or aux-display.
```
위와 같은 문제가 메시지가 출력된다면, `<auto>etc` 폴더에 대한 검색을 실패하여 생긴 문제이다.
따라서 `Config.prc` 파일을 찾지 못하게 되어 디스플레이 라이브러리 설정에 실패한 것을 의미한다.

이를 해결하려면 위에서 언급한 것처럼 prc 파일을 찾도록 폴더 구조를 바꾸거나,
명시적으로 prc 파일을 로드하면 된다.




## Render Pipeline 설정
Render Pipeline 에서는 `resources/config` 폴더에 있는 설정 파일들을 이용하여 라이브러리를 조절한다.
이 파일들은 템플릿 파일이기 때문에, 프로젝트에서 이 템플릿 파일들을 사용하여 설정을 조절하면 된다.

그리고 `resources` 폴더에서 그 외의 다른 폴더 및 파일들을 데이터 파일로써 사용한다.

### base 폴더 및 config 폴더 설정
Render Pipeline 에서는 기본적으로 현재 shared library 파일이 있는 폴더를 기준으로 `../share/render_pipeline` 을
base 경로로 설정한다. 그리고 base 경로를 기준으로 `config` 폴더를 config 경로로 설정한다.

이러한 값은 `MountManager` 에서 `set_base_path` 및 `set_config_dir` 함수를 사용하여 변경할 수 있다.

```
- render_pipeline_cpp
  ├ bin
  ├ etc
  | └ rpsamples
  |   └ default
  ├ ...
  └ share
    └ render_pipeline
      ├ ...
      └ rpplugins
```
예를 들어, 위와 같은 폴더 구조에서 `rpcpp_samples` 프로젝트의 경우 샘플마다 다른 설정을 구성하고,
같은 데이터 폴더를 공유하기 위해서 아래와 같이 설정한다.
```
render_pipeline->get_mount_mgr()->set_base_path("../share/render_pipeline");
render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
```

### PRC 추가 설정
Render Pipeline 에서는 ShowBase 가 생성되기 전에 `panda3d-config.prc` 파일을 추가로
로드하여 위의 설정 값들을 덮어씌운다.

### 플러그인 설정
[플러그인 시스템](plugin_system.md) 문서 참고.
