# Panda3D 파이프라인
**Languages**: [English](../../framework/panda3d_pipeline.md)

참고 사항:
- [Data Loop 및 Node](data_loop_and_node.md).

## Call Tree
### main_loop
```cpp
PandaFramework::main_loop()
{
    while (do_frame()) {}
}

PandaFramework::do_frame()
{
    AsyncTaskManager::poll()
    {
        for_each(AsyncTaskChain)
        {
            AsyncTaskChain::poll()
            {
                for_each(AsyncTask) {}
            }
        }
    }
}
```

### for_each(AsyncTask)
```cpp
// Task "data_loop" has -50 sorts.
PandaFramework::task_data_loop()

// Task "event"
PandaFramework::task_event()

// Task "igloop" has 50 sorts.
PandaFramework::task_igloop()
```

### task_data_loop
```cpp
PandaFramework::task_data_loop()
{
    DataGraphTraverser::traverse()
    {
        if (is_of_type(DataNode::get_class_type())
        {
            DataGraphTraverser::r_transmit()
            {
                DataNode::transmit_data()
                DataGraphTraverser::traverse_below()
            }
        }
        else
        {
            DataGraphTraverser::traverse_below()
        }
    }
}
```

### task_event
```cpp
// Task "event"
PandaFramework::task_event()
{
}
```

### task_igloop
```cpp
PandaFramework::task_igloop()
{
    GraphicsEngine::render_frame()
    {
        GraphicsEngine::do_flip_frame()
        {
            GraphicsEngine::WindowRenderer::do_flip()
            {
                GraphicsEngine::flip_windows()
                {
                    for_each(WINDOWS)
                    {
                        GraphicsOutput::begin_flip()
                        {
                        }
                    }

                    for_each(WINDOWS)
                    {
                        GraphicsOutput::end_flip()
                        {
                            // swap buffer
                        }
                    }
                }
            }
        }

        GraphicsEngine::WindowRenderer::do_frame()
        {
            GraphicsEngine::cull_to_bins();
            GraphicsEngine::cull_and_draw_together();
            GraphicsEngine::draw_bins();
            GraphicsEngine::process_events()
            {
                for_each(WINDOWS)
                {
                    GraphicsOutput::process_events();
                }
            }
        }
    }
}
```
