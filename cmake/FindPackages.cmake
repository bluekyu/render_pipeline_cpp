# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

function(_find_boost required_component)
    if(NOT TARGET Boost::boost)
        option(Boost_USE_STATIC_LIBS    "Boost uses static libraries" OFF)
        option(Boost_USE_MULTITHREADED  "Boost uses multithreaded"  ON)
        option(Boost_USE_STATIC_RUNTIME "Boost uses static runtime" OFF)

        set(BOOST_ROOT "" CACHE PATH "Hint for finding boost root directory")
    endif()

    set(missed_component "")
    foreach(component ${required_component})
        if(NOT TARGET Boost::${component})
            list(APPEND missed_component ${component})
        endif()
    endforeach()

    if(missed_component)
        find_package(Boost 1.62.0 REQUIRED ${missed_component})
    endif()

    if(NOT TARGET Boost::boost)
        find_package(Boost 1.62.0 REQUIRED)
    endif()
endfunction()

_find_boost("filesystem")

if(NOT TARGET panda3d::panda3d)
    # find panda3d
    set(panda3d_ROOT "" CACHE PATH "Hint for finding panda3d root directory")
    find_package(panda3d REQUIRED)
endif()

# find doxygen
option(RENDER_PIPELINE_BUILD_DOCUMENTS "Build doxygen documents" OFF)
option(RENDER_PIPELINE_DOXYGEN_SKIP_DOT "Skip to find Dot for Doxygen" ON)
if(RENDER_PIPELINE_BUILD_DOCUMENTS)
    find_package(doxygen REQUIRED)
    set(DOXYGEN_HAVE_DOT "NO")
    if(NOT RENDER_PIPELINE_DOXYGEN_SKIP_DOT)
        set(DOXYGEN_HAVE_DOT "YES")
    endif()
endif()
