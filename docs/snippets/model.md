# Model

## Bounding Box (Bounding Volume)
### Show
```cpp
NodePath::show_tight_bounds();
```

### Hide
```cpp
NodePath::hide_bounds();
```

### Get
```cpp
LPoint3 min_point;
LPoint3 max_point;
NodePath::calc_tight_bounds(min_point, max_point, OTHER_NODEPATH);
```
