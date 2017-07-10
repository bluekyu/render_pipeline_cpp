# Python Porting

## Standard Python Functions
See a file `render_pipeline/rplibs/py_to_cpp.hpp`



## Panda3D Direct

### Messenger
- Python
```py
messenger.send(EVENT_NAME)
```

- C++
```cpp
throw_event_directly(*EventHandler::get_global_event_handler(), EVENT_NAME);
```

### Notify
- Python
```py
self.notify.debug(LOG_MESSAGE)
```

- C++
```cpp
rppanda_cat.debug() << LOG_MESSAGE << std::endl;
```

### Task Manager
- Python
```py
self.taskMgr.add(TASK_FUNC, TASK_NAME, sort = SORT)
```

- C++
```cpp
ShowBase::add_task(TASK_FUNC, DATA_PTR, TASK_NAME, SORT);
```
