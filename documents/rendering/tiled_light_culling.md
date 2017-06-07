Tiled Light culling 을 수행할 때에는 스테레오 렌더링을 수행하지 않고, mono eye 기준으로 양안에서 동일한 light 를 가질 수 있도록 렌더링을 한다.

먼저, FlagUsedCellsStage 에서 3D 형태로 cell 을 나눈다. width, height 는 light manager 의 tile 개수를 사용하고, depth 는 lighting.culling_grid_slices 값을 사용한다. 그리고 각 셀마다 flag 를 표시하여 사용 여부를 나타낸다.
스테레오의 경우에는 좌, 우 경우를 모두 고려하여 1 또는 2개의 셀에 모두 표시를 한다.

CollectUsedCellsStage 에서는 이전에서 표시한 cell 들에 대해서 indexing 을 수행한다.
