# Task

## Panda3D Native
See https://www.panda3d.org/manual/index.php?title=Tasks&language=cxx



## Render Pipeline
See `TaskManager` in `rppanda/task/task.hpp`

- Add
```cpp
class A
{
    AsyncTask::DoneStatus f(FunctionalTask* task);

    void g()
    {
        auto task_mgr = rpcore::Globals::base->get_task_mgr();
        // OR, TaskManager::get_global_ptr();

        // using bind
        task_mgr->add(std::bind(&A::f, this, std::placeholders::_1), TASK_NAME);
    }

    void h()
    {
        // using lambda
        TaskManager::get_global_ptr()->add([this](rppanda::FunctionalTask* task) {
                g();
                return AsyncTask::DS_cont;
            }, TASK_NAME);
    }

    void i()
    {
        // using lambda and bind to DirectObject instance (ex, ShowBase)
        rpcore::Globals::base->add_task([this](rppanda::FunctionalTask* task) {
                h();
                return AsyncTask::DS_cont;
            }, TASK_NAME);
    }
};
```

- Add with delay

Use `do_method_later` function.

- Remove
```cpp
TaskManager::get_global_ptr()->remove(TASK_NAME);
TaskManager::get_global_ptr()->remove(TASK_PTR);
```
