# list header
set(${PROJECT_NAME}_header_root
    "${PROJECT_SOURCE_DIR}/include/rpplugins/env_probes/env_probes_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpplugins/env_probes/environment_probe.hpp"
    "${PROJECT_SOURCE_DIR}/include/rpplugins/env_probes/probe_manager.hpp"
)

set(${PROJECT_NAME}_headers
    ${${PROJECT_NAME}_header_root}
)

# grouping
source_group("env_probes" FILES ${${PROJECT_NAME}_header_root})



# list source
set(${PROJECT_NAME}_source_root
    "${PROJECT_SOURCE_DIR}/src/apply_envprobes_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/apply_envprobes_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/cull_probes_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/cull_probes_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/env_probes_plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/environment_capture_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/environment_capture_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/environment_probe.cpp"
    "${PROJECT_SOURCE_DIR}/src/probe_manager.cpp"
)

set(${PROJECT_NAME}_sources
    ${${PROJECT_NAME}_source_root}
)

# grouping
source_group("src" FILES ${${PROJECT_NAME}_source_root})
