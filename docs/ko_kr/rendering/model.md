# Panda3D 모델
**Translation**: [English](../../rendering/model.md)

## Egg 포맷 생성
다른 포맷(obj, fbx, ma) 파일 등에서 egg 포맷으로 변환하기 위한 방법으로 아래 Panda3D 매뉴얼을 참조하면 된다.
- https://www.panda3d.org/manual/index.php/Model_Export

### Blender
Blender 에서 변환하기 위해서 Blender 애드온을 설치하여 모델을 egg 포맷으로 변환할 수 있다.

자세한 방법은 [Panda3D 매뉴얼](https://www.panda3d.org/manual/index.php/Converting_from_Blender) 을 참조하면 되고,
현재 지원되고 있는 YABEE 프로젝트에서는 `YABEE_HowTo.pdf` 파일에서 YABEE 플러그인 사용 방법을 확인할 수 있다.

#### 요약
1. [YABEE github](https://github.com/09th/YABEE) 페이지에서 프로젝트 다운로드
   - **Clone or download** 버튼을 누르고 **Download ZIP** 눌러서 다운로드
2. Blender 프로그램을 열어서 **File - User Preferences** 창을 연다.
3. **Addons** 메뉴에서 아래에 있는 **Install from File...** 을 클릭한다.
4. 1에서 다운로드 받은 ZIP 파일을 선택하여 설치한다.
5. **Import-Export: Panda3d EGG format** 항목이 나타나고, 체크박스를 선택한다.
6. **Save User Settings** 클릭하여 설정을 저장한다.
7. **File - Export - Panda3d (.egg)** 를 메뉴가 생성되고, 이를 이용하여 egg 포맷으로 내보낼 수 있다.



## Bam 포맷 생성
Bam 포맷으로 변환은 `egg2bam` 도구를 사용하여 Egg 포맷으로부터 변환이 가능하다.
자세한 방법은 아래 Panda3D 매뉴얼을 참조하면 된다.
- https://www.panda3d.org/manual/index.php/Converting_Egg_to_Bam
