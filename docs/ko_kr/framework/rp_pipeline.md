# Render Pipeline C++ 의 파이프라인
**Languages**: [English](../../framework/rp_pipeline.md)

## 리소스 릴리즈
Panda3D 에게 리소스(NodePath, ReferenceCount)를 넘겨주는 구조로 인하여,
Render Pipeline 및 ShowBase, 그리고 플러그인들의 메모리 관리에 주의해야 한다.

### Render Pipeline
Render Pipeline 에서 ShowBase 없이 시작할 경우, ShowBase 를 직접 생성하고 관리하는 형태를 가진다.
따라서 RenderPipeline 인스턴스가 릴리즈 되기 전에 ShowBase 가 릴리즈 된다.

만일, ShowBase 를 넘겨주었다면, RenderPipeline 인스턴스에서는 이를 관리하지 않는다.

#### 플러그인
플러그인의 경우 DLL 이 언로드 되기 전에 모든 리소스들을 해제해야 한다.
그렇지 않을 경우, 다른 리소스에서 플러그인 DLL 로부터 온 리소스를 해제할 때, DLL 이 존재하지 않아서 에러를 일으킨다.

이를 쉽게 처리하기 위해서, Render Pipeline 의 PluginManager 에서는 로드된 DLL 을 Render Pipeline DLL 이
언로드 되기 전까지 삭제하지 않는다. 즉, `PluginManager::Impl::plugin_creators_` static 변수를 통해서 DLL을 관리한다.
이를 통해, DLL을 언로드기 하기 전에 모든 리소스를 해제하는데, 문제의 여지를 줄일 수 있다.

만일, 해당 DLL 들을 강제로 언로드 시키고 싶다면, `PluginManager::release_all_dll()` 함수를 사용하여 언로드가 가능하다.
