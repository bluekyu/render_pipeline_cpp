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



## Render Pipeline 설정
Render Pipeline 에서는 `resources/config` 폴더에 있는 설정 파일들을 이용하여 라이브러리를 조절한다.

### PRC 추가 설정
Render Pipeline 에서는 ShowBase 가 생성되기 전에 `panda3d-config.prc` 파일을 추가로
로드하여 위의 설정 값들을 덮어씌운다.

### 플러그인 설정
[플러그인 시스템](plugin_system.md) 문서 참고.
