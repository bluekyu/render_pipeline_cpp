# Panda3D Event

## General Event
```cpp
rpcore::Globals::base->accept(EVENT_NAME, [](const Event* ev, void* data) {
        ...
    }, DATA_PTR);
```

## Keyboard
We can use `rpcore::Globals::base::accept` and `PandaFramework::define_key`.

See also https://www.panda3d.org/manual/index.php?title=Keyboard_Support&language=cxx
