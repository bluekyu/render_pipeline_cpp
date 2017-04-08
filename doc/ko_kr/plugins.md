# Plugins   {#ko_kr_render_pipeline_plugins}

## OpenVR
OpenVR (https://github.com/ValveSoftware/openvr) 라이브러리를 위한 플러그인이다. OpenVR 플러그인이 활성화 되어 있으면,
플러그인 로드 단계에서 OpenVR 를 초기화 한다. 그리고 OpenVRRenderStage 에서 마지막에 그려진 화면을 OpenVR texture 에 복사하고,
매 프레임 마지막 단계에서 해당 texture 를 HMD 에 보낸다.

OpenVR 플러그인을 사용할 때, `pipeline.yaml` 설정 파일에 `stereo_mode` 값이 true 이어야 한다.
또한, 현 플러그인의 한계로 인하여 `Config.prc` 파일에 `win-size` 값이 사용하려면 HMD 해상도에 맞게 설정이 되어 있어야 한다.
