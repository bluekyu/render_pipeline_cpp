# Pipeline Architecture {#ko_kr_pipeline_architecture}

Render Pipeline 에서 pipeline 구조에 따라 렌더링 방식과 순서를 정의한다.

## Pipeline 구조
Pipeline 에서 stage 는 렌더링 실행 단위로 `etc/render_pipeline/config/stages.yaml` 파일에서 정의된
순서에 따라서 stage 가 실행된다.

- @subpage stage_system
