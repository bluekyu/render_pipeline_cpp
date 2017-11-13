# Data Loop 및 Node
**Translation**: [English](../../framework/data_loop_and_node.md)

Panda3D 에서 "data_loop" 이름의 task 가 존재한다. 해당 task 에서는 data 처리를 위한 그래프 순회를 수행한다.

해당 graph 에서는 `DataNode` 를 제외한 나머지 노드는 pass 하고, `DataNode` 에 대해서는
데이터를 처리하고 이를 자식으로 전달한다.

DataNode 에서는 `define_input` 과 `define_output` 을 사용하여, 받을 데이터와 보내줄 데이터를 정의한다.

이를 사용하는 대표적인 예로 키보드 및 마우스 데이터 처리가 있다.

## 키보드 및 마우스 데이터
키보드 및 마우스 데이터는 `MouseAndKeyboard` 노드를 통해서 처리가 된다.
이는 `PandaFramework::get_mouse()` 함수에서 생성되고 root 노드에 붙여진다.

그리고 `WindowFramework::get_mouse()` 함수에서 `MouseWatcher` 노드를 자식으로 붙이고,
`MouseWatcher` 를 통해 마우스 데이터에 접근하는 인터페이스를 제공한다.

`MouseAndKeyboard` 노드에서는 `do_transmit_data` 함수에서 `GraphicsWindow` 로부터 버튼 이벤트가 있는지 확인하고,
이를 모아서 "button_events" 이름으로 output 을 내놓는다.

여기까지는 데이터를 처리 및 전달할 뿐이며, 위 데이터를 Panda3D 의 이벤트(예, "mouse1-up" 등)로 보내주기 위해
`WindowFramework` 에서 이벤트 처리용 DataNode 를 생성하고, 이를 `WindowFramework::get_mouse()` 노드의 자식으로 붙인다.

키보드 이벤트는 `WindowFramework::enable_keyboard` 의 `ButtonThrower` 노드에서 처리되고,
마우스 이벤트는 `WindowFramework::setup_trackball` 의 `Trackball` 노드에서 처리된다.

### 윈도우 버튼 이벤트
Low level 윈도우 이벤트는 `ButtonHandle` 객체를 통해 `GraphicsWindowInputDevice` 객체로 전달되고,
이를 `ButtonEvent` 에 담아서 관리한다.

참고로 마우스 버튼의 핸들은 `MouseButton` 클래스에서 미리 정의된 객체를 사용하며,
키보드 버튼의 핸들은 `KeyboardButton` 클래스에서 미리 정의된 객체를 사용한다.

이후 `GraphicsWindowInputDevice` 객체는 `ButtonEvent` 들을 리스트로 관리하며, `GraphicsWindow::get_button_event` 를
통해서 값을 리턴한다. 그리고 위에서 언급한 것처럼 `MouseAndKeyboard` 에서 리턴된 `ButtonEvent` 를
모아서 output 으로 저장한다.
