cmake_minimum_required(VERSION 2.8.2)


include("${GN_dir_gremlin}/gremlin.cmake")
project(libcds-download NONE)
GN_loader("${args}"
    GIT_REPOSITORY      https://github.com/khizmax/libcds.git
    GIT_TAG             master
    SOURCE_DIR          "${root}/src"
    BINARY_DIR          "${root}/build"
    CMAKE_ARGS          
        -DCMAKE_CXX_FLAGS=-DCDS_BUILD_LIB
        -DCMAKE_INSTALL_PREFIX=${root}/install
    )
