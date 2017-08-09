# Virtual File System
**Translation**: [English](../virtual_filesystem)

## Panda3D 파일 시스템


## Render Pipeline 파일 시스템
Render Pipeline에서는 Panda3D의 Virtual File System에 가상 경로를 추가하여, 플러그인 및 데이터에 쉽게 접근 할 수 있도록 한다.

추가하는 가상 경로를 설정하는 것은 rpcore::MountManager 에서 담당한다. Render Pipeline를 일반적으로 설치하면,
`share/render_pipeline` 폴더 아래에 데이터들이 설치되는데, 여기가 `base` 폴더가 된다.
그리고 `etc` 폴더 아래에 프로젝트 설정 파일들이 추가되는데, 여기가 `config` 폴더가 된다.

예시 코드는 아래와 같다.
```cpp
render_pipeline->get_mount_mgr()->set_base_path("../share/render_pipeline");
render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
```

### 마운트 경로
- `/$$rp`: `MountManager::set_base_path`에 의해 설정된 경로. 데이터가 저장되어 있다.
- `/$$rp/shader`: `MountManager::set_base_path` 경로 아래에 `rpcore/shader` 경로.
Render Pipeline 쉐이더가 저장되어 있다.
- `/$$rpconfig`: `MountManger::set_config_dir`에 의해 설정된 경로. 설정 파일들이 저장되어 있다.
- `/$$rptemp`: `MountManager::set_write_path`에 의해 설정된 경로. 임시 폴더로 사용되며,
쉐이더 템플릿에서 만든 쉐이더 등이 저장되어 있다. 미설정시에는 Ram Disk를 사용한다.

### 사용 방법
MountManager는 Render Pipeline에서 기본적으로 생성을 한다. 만일, Render Pipeline을 쓰지 않는 경우에는
아래와 같이 직접 설정할 수 있다.
```cpp
rpcore::MountManager mount_manager;
mount_manager.set_base_path("../share/render_pipeline");
mount_manager.mount();
```

마운트된 경로는 아래와 같이 사용할 수 있다.
```cpp
base_.get_loader()->load_model("/$$rp/resources/music-box/models/MusicBox");
```
