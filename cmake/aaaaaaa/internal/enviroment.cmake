## --------------------------| enviroment |-------------------------- ##
## -----------| common

macro(GN_setupEnviroment)
    # place exe and libs into one direcory to simplify their linking
    get_filename_component(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" ABSOLUTE CACHE)
    get_filename_component(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" ABSOLUTE CACHE)
    get_filename_component(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" ABSOLUTE CACHE)
    link_directories("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    # init project
    set(GN_dir_building ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "")
    set(GN_dir_solution ${PROJECT_SOURCE_DIR}       CACHE STRING "")
    endmacro()
