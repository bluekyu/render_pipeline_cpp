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

# find panda3d
set(PANDA3D_ROOT "" CACHE PATH "Hint for finding panda3d root directory")
find_package(Panda3d REQUIRED COMPONENTS libp3framework libpanda libpandaexpress libp3dtool libp3dtoolconfig libp3direct libp3interrogatedb)

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
