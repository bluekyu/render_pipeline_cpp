# Physically Based Shading 모델
**Translation**: [English](../../rendering/pbs-model.md)

## Blender 에서 Physically Based Shading 속성 사용하기
Physically Based Shading 속성들은 Panda3D material 에 저장이 되지만, 현재 Blender 에서만 이러한 속성을 직접 수정할 수 있다.

Blender 에서 Physically Based Shading 속성을 보기 위해서는 아래 플러그인 설치가 필요하다.
- https://github.com/tobspr/Panda3D-Bam-Exporter

프로젝트 README 에 따라 플러그인을 설치하면, export 항목에 **Panda3D (.bam)** 항목이 생기고 이를 사용하면 된다.
만일, Egg 포맷으로 export 하고 싶다면, YABEE 플러그인을 사용하여 export 할 때, **Export PBS** 옵션을 선택해주면 된다.

### 요약
1. [Panda3D BAM Exporter github](https://github.com/tobspr/Panda3D-Bam-Exporter) 페이지에서 프로젝트 **클론**
   - git 을 사용해서 `%appdata%\Blender Foundation\Blender\[VERSION]\scripts\addons` 폴더 아래에서 아래 명령 수행
     - `[VERSION]` 은 현재 설치된 blender 버전
   - `git clone --recursive https://github.com/tobspr/Panda3D-Bam-Exporter.git`
2. Blender 프로그램을 열어서 **File - User Preferences** 창을 연다.
3. **Addons** 메뉴에서 "panda" 를 검색하여 **Import-Export: Panda3d BAM Export (PBE)** 항목을 찾고 체크박스를 선택한다.
4. **Save User Settings** 클릭하여 설정을 저장한다.
5. **File - Export - Panda3d (.bam)** 를 메뉴가 생성되고, 이를 이용하여 PBS 속성을 포함한 Bam 포맷으로 내보낼 수 있다.
6. Egg 포맷에서는 **File - Export - Panda3d (.egg)** 메뉴에서 **Export PBS** 옵션을 선택하면 된다.
