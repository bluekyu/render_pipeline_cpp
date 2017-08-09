# 스테레오 및 가상현실
**Translation**: [English](../../rendering/stereo-and-vr)

## 스테레오 렌더링
Render Pipeline 에서 스테레오 렌더링을 사용하려면 stereo 옵션을 활성화 해야 한다.
Render Pipeline 의 설정 파일 중 `pipeline.yaml` 파일을 열어서 `stereo_mode` 옵션을 `true` 로 변경한다.

### Panda3D 방식과의 차이점
Panda3D 에서도 자체적으로 스테레오 렌더링을 지원한다. 그러나 Panda3D 에서 지원하는 방식은 렌더링하는 디스플레이와
카메라 렌즈를 좌/우로 나누어서 단순히 2번 렌더링하는 것이다.

이러한 방법으로는 Deferred Rendering 을 수행할 수 없기 때문에 Panda3D 의 자체적인 스테레오 기능을 사용하지 않는다.
또한, Panda3D 의 스테레오 렌즈 기능들 역시 사용하지 않는다.



## 가상현실 및 HMD 렌더링
Render Pipeline 에서 가상현실을 위한 Head Mount Display (HMD) 렌더링을 지원한다.
렌더링 자체는 위에서 언급된 스테레오 렌더링을 사용하여 수행되며, HMD 장치에 디스플레이 하는 것은 플러그인을 통해서 지원한다.

현재 지원하고 있는 플러그인은 OpenVR 플러그인으로, 아래 주소에서 사용이 가능한다.

    Plugins for Render Pipeline C++: https://github.com/bluekyu/rpcpp_plugins

OpenVR 플러그인을 설치한 후, OpenVR 플러그인을 `plugins.yaml` 에서 활성화 시킨다.
그리고 `pipeline.yaml` 파일에서 `stereo_mode` 옵션을 `true` 로 변경한다.

추가적으로, 렌더링 버퍼 크기를 설정해줘야 한다. 기본적인 OpenVR 렌더링 `넓이 x 높이` 는 `1512 x 1680` 이다.
이를 `Config.prc` 파일에서 창크기 설정인 `win-size` 에 반영해도 된다.

그러나 일반적으로 모니터의 해상도가 이보다 작기 때문에 창 크기가 범위를 넘어간다.
이를 해결하려면, `pipeline.yaml` 파일에서 `resolution_scale` 값을 `0.0` 으로 설정하고,
`resolution_width` 값과 `resolution_height` 값을 각각 `1512`, `1680` 으로 설정한다.

그러면 창크기는 유지한채 렌더링 해상도만 바꿀 수 있고, 마지막 단계에서 윈도우 크기에 맞게 축소하거나 자른다.
자르는 방식을 사용하려면 `pipeline.yaml` 파일에서 `screen_cropping` 값을 `true` 로 설정한다.

### 요약
- `plugins.yaml`: 플러그인 활성화 (예, `openvr`)
- 창크기 변경 `Config.prc` 수정
  - `win-size` 에 `width x height` 적용
- 창크기 고정: `pipeline.yaml` 수정
  - `stereo_mode`: true
  - `resolution_scale`: 0.0
  - `resolution_width` 및 `resolution_height` 에 `width x height` 적용
  - 창크기에 맞게 자르기(cropping): `screen_cropping` 값을 `true` 로 변경
