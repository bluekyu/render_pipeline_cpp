# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

# find boost
option(Boost_USE_STATIC_LIBS    "Boost uses static libraries" OFF)
option(Boost_USE_MULTITHREADED  "Boost uses multithreaded"  ON)
option(Boost_USE_STATIC_RUNTIME "Boost uses static runtime" OFF)

set(BOOST_ROOT "" CACHE PATH "Hint for finding boost root directory")
set(BOOST_INCLUDEDIR "" CACHE PATH "Hint for finding boost include directory")
set(BOOST_LIBRARYDIR "" CACHE PATH "Hint for finding boost library directory")

find_package(Boost 1.62.0 REQUIRED COMPONENTS filesystem system)
if(Boost_FOUND)
    message(STATUS "Boost include path: ${Boost_INCLUDE_DIRS}")
    if(NOT Boost_USE_STATIC_LIBS)
        set(Boost_LIBRARIES Boost::dynamic_linking ${Boost_LIBRARIES})
    endif()
endif()

# find panda3d
set(PANDA3D_ROOT "" CACHE PATH "Hint for finding panda3d root directory")

find_package(Panda3d REQUIRED COMPONENTS libp3framework libpanda libpandaexpress libp3dtool libp3dtoolconfig libp3direct libp3interrogatedb)
if(Panda3d_FOUND)
    message(STATUS "Panda3D include path: ${Panda3d_INCLUDE_DIRS}")
endif()

# find yaml-cpp
set(YAMLCPP_ROOT "" CACHE PATH "Hint for finding yaml-cpp root directory")
set(YAMLCPP_INCLUDEDIR "" CACHE PATH "Hint for finding yaml-cpp include directory")
set(YAMLCPP_LIBRARYDIR "" CACHE PATH "Hint for finding yaml-cpp library directory")
option(YAMLCPP_USE_STATIC_LIBS "Yaml-cpp uses static libraries" OFF)
option(YAMLCPP_USE_MULTITHREADED "Yaml-cpp uses non-multithreaded libraries ('mt' tag)" ON)

find_package(YamlCpp REQUIRED)
if(YamlCpp_FOUND)
    message(STATUS "yaml-cpp include path: ${YamlCpp_INCLUDE_DIRS}")
endif()

# find freetype
find_package(FreeType REQUIRED)
if(FREETYPE_FOUND)
    message(STATUS "freetype include path: ${FREETYPE_INCLUDE_DIRS}")
endif()

# find spdlog
set(SPDLOG_ROOT "" CACHE PATH "Hint for finding spdlog root directory")
set(SPDLOG_INCLUDEDIR "" CACHE PATH "Hint for finding spdlog include directory")

find_package(Spdlog REQUIRED)
if(Spdlog_FOUND)
    message(STATUS "spdlog include path: ${Spdlog_INCLUDE_DIRS}")
endif()

# find doxygen
option(RPCPP_BUILD_DOCUMENTS "Build doxygen documents" OFF)
option(RPCPP_DOXYGEN_SKIP_DOT "Skip to find Dot for Doxygen" ON)
if(RPCPP_BUILD_DOCUMENTS)
    find_package(doxygen REQUIRED)
    set(DOXYGEN_HAVE_DOT "NO")
    if(NOT RPCPP_DOXYGEN_SKIP_DOT)
        set(DOXYGEN_HAVE_DOT "YES")
    endif()
endif()
