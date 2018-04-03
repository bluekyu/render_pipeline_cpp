# Intervals

## Panda3D Native
See [Panda3D Intervals and Sequences document]( https://www.panda3d.org/manual/index.php/Using_Intervals_to_move_the_Panda)



## Render Pipeline
You can use `Interval` classes in `render_pipeline/rppanda/interval.hpp` which are similar to Python API.

And `NodePath::*Interval` extensions can be replaced by `Lerp*Interval` classes.

For example,
```py
# NodePath np;
interval = np.hprInterval(7, (360, 0, 0))
```

These codes can be converted to

```cpp
NodePath np;
PT(rppanda::LerpHprInterval) interval = new rppanda::LerpHprInterval(np, 7.0f, LVecBase3(360, 0, 0));
```

You can see more examples (ex, music-box or looking-and-gripping) in
[Panda3D samples in rpcpp_samples repository](https://github.com/bluekyu/rpcpp_samples/tree/master/panda3d_samples).
