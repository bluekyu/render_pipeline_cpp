# Panda3D Event and Task

## General Event
```cpp
rpcore::Globals::base->accept(EVENT_NAME, [](const Event* ev, void* data) {
        ...
    }, DATA_PTR);
```

## Keyboard Event
We can use `rpcore::Globals::base::accept` and `PandaFramework::define_key`.

See also https://www.panda3d.org/manual/index.php?title=Keyboard_Support&language=cxx


## Task
```cpp
rpcore::Globals::base->add_task([](const GenericAsyncTask* task, void* user_data) {
        ...
    }, DATA_PTR, TASK_NAME);
```
