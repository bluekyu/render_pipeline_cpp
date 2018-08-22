# C++ Guides for Panda3D
**Languages**: [English](../../framework/cpp_guides.md)

## 메모리
- Reference Counting: https://www.panda3d.org/manual/index.php/Reference_Counting

### DeletedChain 사용
Panda3D 에서 많이 사용되며 자주 삭제 및 생성되는 객체들의 메모리 관리 효율을 높이기 위한 기능을 제공한다.

Panda3D type (`TypeHandle`) 이 존재하는 클래스에서 `ALLOC_DELETED_CHAIN` 을 사용하면,
해당 클래스를 생성하면 linked list 로 관리되는 메모리 영역에 인스턴스를 생성한다.
그리고 해당 인스턴스가 삭제되면 메모리를 제거하지 않고, 영구적으로 남겨두며 이후 생성되는 객체를 해당 메모리에 다시 생성한다.

주의할 점은 상위 클래스가 DeletedChain 을 가질 경우, 해당 클래스도 반드시 `ALLOC_DELETED_CHAIN` 를 추가해주어야 한다.

자세한 내용은 "deletedChain.h" 파일의 주석을 참고하면 된다.



## 상속
### TypedObject 상속
Panda3D 에서는 RTTI 를 사용하는 대신, 직접 type information 을 구현한다.
`TypedObject` 클래스는 이를 지원하기 위한 형태로, 아래 함수들을 구현하여 클래스의 타입을 만들 수 있다.

```cpp
public:
    virtual TypeHandle get_type() const { return get_class_type(); }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

PUBLISHED:
    static TypeHandle get_class_type() { return type_handle_; }

public:
    static void init_type() {
        // ...
    }

private:
    static TypeHandle type_handle_;
```

위의 구현과 함께, `ConfigureFn` 함수 안에서 클래스의 `init_type` 함수를 호출해주면 된다.

자세한 내용은 "typedObject.h" 파일의 주석을 참고하면 되며, Render Pipeline 에서 사용을 하고 있으므로 이를 참조하면 된다.
