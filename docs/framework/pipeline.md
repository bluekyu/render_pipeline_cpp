# Pipeline

## Call Tree
```cpp
// Task "data_loop" has -50 sorts.
PandaFramework::task_data_loop()
{
}

// Task "event"
PandaFramework::task_event()
{
}

// Task "igloop" has 50 sorts.
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
