# Event

## Panda3D Native
See `EventHandler` class in `eventHandler.h`

- Add hook (listener)
```cpp
class A
{
    static void f(const Event* ev)
    {
        ...
    }

    static void g(const Event* ev, void* data)
    {
        A* self = reinterpret_cast<A*>(data);
        ...
    }

    void h()
    {
        auto handler = EventHandler::get_global_event_handler();

        // using static member function
        handler->add_hook(EVENT_NAME, f);

        // using static memeber function with 'this' pointer
        handler->add_hook(EVENT_NAME, g, this);
    }

    void i()
    {
        auto handler = EventHandler::get_global_event_handler();

        // using lambda
        handler->add_hook(EVENT_NAME, [](const Event* ev) {
            ...
        });

        // using lambda with 'this' pointer
        handler->add_hook(EVENT_NAME, [](const Event* ev, void* data) {
            A* self = reinterpret_cast<A*>(data);
            ...
        }, this);
    }
};
```

- Remove hook (listener)
```cpp
EventHandler::get_global_event_handler()->remove(EVENT_NAME);
EventHandler::get_global_event_handler()->remove(EVENT_NAME, FUNCTION_PTR);
EventHandler::get_global_event_handler()->remove(EVENT_NAME, FUNCTION_PTR, DATA_PTR);
```

- Send event
See `throw_event.h` in Panda3D

```cpp
#include <throw_event.h>

// queuing
throw_event(EVENT_NAME);
throw_event(EVENT_NAME, PARAM1);
...

// run immediately
throw_event_directly(HANDLER, EVENT_NAME);
throw_event_directly(HANDLER, EVENT_NAME, PARAM1);
...
```

### Keyboard Event
We can use hook functions or `PandaFramework::define_key`.

See also https://www.panda3d.org/manual/index.php?title=Keyboard_Support&language=cxx



## Render Pipeline
See `Messenger` in `rppanda/showbase/messenger.hpp`

- Add hook (listener)
```cpp
class A
{
    void f(const Event* ev);

    void g()
    {
        auto messenger = rpcore::Globasl::base->get_messenger();
        // OR, Messenger::get_global_instance();

        // using bind
        messenger->accept(EVENT_NAME, std::bind(&A::f, this, std::placeholders::_1));
    }

    void h()
    {
        // using lambda
        Messenger::get_global_instance()->accept(EVENT_NAME, [](const Event* ev) { ... });

        // using lambda with 'this' pointer
        Messenger::get_global_instance()->accept(EVENT_NAME, [this](const Event* ev) { g(); });
    }

    void i()
    {
        // using lambda and bind to DirectObject instance (ex, ShowBase)
        rpcore::Globals::base->accept(EVENT_NAME, [this](const Event* ev) { h(); });
    }
};
```

- Remove hook (listener)
```cpp
Messenger::get_global_ptr()->ignore(EVENT_NAME);

// only ignore EVENT_NAME bound to the DirectObject instance (ex, ShowBase)
rpcore::Globals::base->ignore(EVENT_NAME);
```
You need to use all EVENT_NAME

- Send event
```cpp
// queuing
Messenger::get_global_instance()->send(EVENT_NAME, true);
Messenger::get_global_instance()->send(EVENT_NAME, PARAM1, true);

// run immediately
Messenger::get_global_instance()->send(EVENT_NAME);
Messenger::get_global_instance()->send(EVENT_NAME, PARAM1);
...
```
