## --------------------------| unit |-------------------------- ##
## -----------| constructor

# newUnit creates an empty unit
macro(GN_newUnit _result name)
    # create a project
    GN_makeUnit(unit ${name})
    # set it's name
    set(${unit}_name ${name})
    # parse constructor's arguments
    GN_parseArgs(${unit} ${ARGN})
    # setup directories
    GN_parseDirs(${unit})
    # return unit
    set(${_result} ${unit})
    endmacro()

# makeUnit defines a new project and returns it's identifer 
macro(GN_makeUnit _result name)
    GN_project(${name})
    set(${_result} ${name})
    endmacro()

# parseArgs parse unit's parameters
macro(GN_parseArgs unit)
    # parse arguments
    set(OPTIONS "")
    set(VALUES  "bFlat;Mode")
    set(ARRAYS  "Units;Private;Public;Libs;Definitions")
    cmake_parse_arguments(${unit}
        "${OPTIONS}" 
        "${VALUES}"
        "${ARRAYS}" ${ARGN})

    # default values
    GN_default(${unit}_bFlat off)
    GN_default(${unit}_Mode "lib")
    endmacro()

# prseDirs setups main direcctories
macro(GN_parseDirs unit)
    # setup main directories
    set(${unit}_dir_root ${PROJECT_SOURCE_DIR})
    if (${unit}_bFlat)
        set(${unit}_dir_private "${${unit}_dir_root}/.")
        set(${unit}_dir_public  "${${unit}_dir_root}/.")
    else()
        set(${unit}_dir_private "${${unit}_dir_root}/${GN_dir_private}")
        set(${unit}_dir_public  "${${unit}_dir_root}/${GN_dir_public}")
        endif()
    # deduct category
    file(RELATIVE_PATH ${unit}_category "${GN_dir_solution}" "${${unit}_dir_root}/..")
    endmacro()

 ## ----------| scanners

 function(GN_scanUnits _result unit)
    # join depended units with all their's depened ones
    GN_getList(units "${${unit}_Units}" "_units" on "${${unit}_Units}")
    GN_return("${units}")
    endfunction()

function(GN_scanLibs _result unit)
    # join depened libs with all depened unit's ones
    GN_getList(libs "${${unit}_units}" "_Libs" off "${${unit}_Libs}")
    GN_return("${libs}")
    endfunction()

function(GN_scanDefs _result unit)
    # join depened definetions with unit's ones
    GN_getList(defs "${${unit}_units}" "_Defs" on "${${unit}_Definitions}")
    # assign them to a current unit
    add_definitions(${defs})
    # return all found defs
    GN_return("${defs}")
    endfunction()

function(GN_scanSources _result dir root)
    # add all files from the directory
    file(GLOB_RECURSE files "${dir}/*")
    # assign them to a current unit
    source_group(TREE ${root} FILES ${files})
    # return all found files
    GN_return("${files}")
    endfunction()

function(GN_listDirsPublic _result unit)
    set(dirs 
        ${${unit}_Public} 
        ${${unit}_dir_public})
    GN_return("${dirs}")
    endfunction()

function(GN_listDirsPrivate _result unit)
    GN_getList(dirs "${${unit}_units}" "_srcs" on 
        ${${unit}_Private} 
        ${${unit}_dir_private})
    GN_return("${dirs}")
    endfunction()

## -----------| targets

function(GN_getMode _result unit)
    set(mode ${${unit}_Mode})
    if (mode STREQUAL "lib")
        list(LENGTH ${unit}_src_private len)
        if (
                ( ${${unit}_bFlat}  )
            OR  ( ${len} EQUAL 0    )
        )
            set(mode "heads")
            endif()
        endif()
    GN_return(${mode})
    endfunction()

macro(GN_asApp unit)
    # create an executable
    add_executable(${unit}
        ${${unit}_src_private}
        ${${unit}_src_public})
    # set as unlinkable
    set(${unit}_bLink off)
    # link libraries
    target_link_libraries(${unit} ${${unit}_libs})
    # set debug directory
    set_target_properties(${unit} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${GN_dir_solution})
    # add dependencies
    foreach(unit ${${unit}_units})
        add_dependencies(${unit} ${unit})
        endforeach()
    endmacro()

macro(GN_asLib unit)
    # create a static library target
    add_library(${unit}
        ${${unit}_src_private} 
        ${${unit}_src_public})
    # set as linkable
    set(${unit}_bLink on)
    endmacro()

macro(GN_asHeades unit)
    # it's an only way to create a target with no src
    add_custom_target(${unit} SOURCES ${${unit}_src_public})
    # disable linking
    set(${unit}_bLink off)
    endmacro()

## -----------| postprocessing

function(GN_setupIncludes unit)
    # includes all directories
    foreach(dir ${${unit}_dirs_private} ${${unit}_dirs_public})
        include_directories(${dir})
        endforeach()
    endfunction()

function(GN_define unit)
    # define libraly
    if (${unit}_bLink)
        # include self to @_Libs
        list(APPEND ${unit}_Libs ${unit})
        endif()

    # set global properties
    GN_set(${unit}_units "${${unit}_units}")
    GN_set(${unit}_Defs  "${${unit}_Definitions}")
    GN_set(${unit}_Libs  "${${unit}_Libs}")
    GN_set(${unit}_srcs  "${${unit}_dirs_public}")
    endfunction()
