# list render_pipeline/
set(rpassimp_headers
)

# grouping



# list src/
set(rpassimp_sources
    "${PROJECT_SOURCE_DIR}/assimpLoader.cxx"
    "${PROJECT_SOURCE_DIR}/assimpLoader.h"
    "${PROJECT_SOURCE_DIR}/config_assimp.cxx"
    "${PROJECT_SOURCE_DIR}/config_assimp.h"
    "${PROJECT_SOURCE_DIR}/loaderFileTypeAssimp.cxx"
    "${PROJECT_SOURCE_DIR}/loaderFileTypeAssimp.h"
    "${PROJECT_SOURCE_DIR}/pandaIOStream.cxx"
    "${PROJECT_SOURCE_DIR}/pandaIOStream.h"
    "${PROJECT_SOURCE_DIR}/pandaIOSystem.cxx"
    "${PROJECT_SOURCE_DIR}/pandaIOSystem.h"
    "${PROJECT_SOURCE_DIR}/pandaLogger.cxx"
    "${PROJECT_SOURCE_DIR}/pandaLogger.h"
)

# grouping
source_group("src" FILES ${rpassimp_sources})
