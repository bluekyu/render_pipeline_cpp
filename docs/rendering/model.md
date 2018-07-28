# Panda3D Model
**Languages**: [한국어](../ko_kr/rendering/model.md)

## Loading Models
We provide similar API in Python version.

```cpp
// load single model
NodePath model = get_loader()->load_model("model1.bam");

// load multiple models
std::vector<NodePath> models = get_loader()->load_model({
    "model1.bam",
    "model2.bam"});
```

For asynchronous loading, you can use `load_model_async` API.
```cpp
get_loader()->load_model_async(
    "model1.bam",
    {},
    boost::none,
    false,
    boost::none,
    [](const std::vector<NodePath>& models) {
        // process after loading is finished.
    });
```

For details, see also the below Panda3D manual:
- [Panda3D Manual: Loading Models](https://www.panda3d.org/manual/index.php/Loading_Models)



## Generating Egg format



### Blender





#### Summary











## Bam 포맷 생성
