cmake_minimum_required(VERSION 3.15)

## --------------------------| variables |-------------------------- ##
## -----------| common settings
set(GN_bDebug       off             CACHE BOOL "prints debug information")
set(GN_cpp_version  17              CACHE BOOL "c++ standart")
## -----------| directories
set(GN_dir_private  "private"       CACHE STRING "private code directory")
set(GN_dir_public   "public"        CACHE STRING "public code directory")
set(GN_dir_tests    "tests"         CACHE STRING "directory with test files")
## -----------| enabled modules
set(GN_modules_avaliable guards PCH projects tests unity)
set(GN_modules_enabled   guards PCH projects tests unity)


# include internal functions
set(GN_dir_gremlin "${CMAKE_CURRENT_LIST_DIR}" CACHE STRING "" FORCE)
include("${GN_dir_gremlin}/internal/enviroment.cmake")
include("${GN_dir_gremlin}/internal/helpers.cmake")
include("${GN_dir_gremlin}/internal/modules.cmake")
include("${GN_dir_gremlin}/internal/unit.cmake")

# include enabled modules
foreach(module ${GN_modules_enabled})
    include("${GN_dir_gremlin}/modules/${module}.cmake")
    endforeach()


## --------------------------| interface |-------------------------- ##

macro(GN_Solution name)
    GN_project(${name})
    GN_setupEnviroment()
    endmacro()

macro(GN_Configure)
    
    endmacro()

macro(GN_Subprojects)
    foreach(path ${ARGN})
        ADD_SUBDIRECTORY(${path})
        endforeach()
    endmacro()


## creates a unit with the folowing params:
#   \ Name                  - unit name
#   \ Units         = {}    - list of depending units               | inherits
#   \ Private       = {}    - list of private external include dirs | 
#   \ Public        = {}    - list of public  external include dirs | inherits
#   \ Libs          = {}    - list of depending external libs       | inherits
#   \ Definitions   = {}    - list of preprocessor defenitions      | inherits
#   \ bFlat         = off   - [on|off] whether the unit uses separated public/private/test directories
#   \ Mode          = lib   - [...] type of unit will be built
#       \ lib       - create a static library
#       \ app       - create an executable
function(GN_Unit Name)
    # create a new unit
    GN_newUnit(unit ${Name} ${ARGN})
    GN_debug("-----------------------------" "-----------------------------")
    GN_debug("Name" "${${unit}_name}")
    GN_debug("Mode" "${${unit}_Mode}")
    GN_debug("Root" "${${unit}_dir_root}")

    # scan depending units
    GN_scanUnits(${unit}_units ${unit})
    GN_debug("units" "${${unit}_units}")
    
    # scan sources and add to a project tree
    GN_scanSources(${unit}_src_private ${${unit}_dir_private} ${${unit}_dir_root})
    GN_scanSources(${unit}_src_public  ${${unit}_dir_public}  ${${unit}_dir_root})
    GN_debug("public files" "${${unit}_src_public}")
    GN_debug("private files" "${${unit}_src_private}")
    
    # scan dependences
    GN_scanLibs(${unit}_libs ${unit})
    GN_scanDefs(${unit}_defs ${unit})
    GN_debug("libs" "${${unit}_libs}" 2)
    GN_debug("defs" "${${unit}_defs}")

    # construct list of directories
    GN_listDirsPublic (${unit}_dirs_public  ${unit})
    GN_listDirsPrivate(${unit}_dirs_private ${unit})
    GN_debug("public dirs" "${${unit}_dirs_public}")
    GN_debug("private dirs" "${${unit}_dirs_private}")

    # call onConfigure events
    GN_callEvent("onConfigure" ${unit})

    # detect a current mode
    GN_getMode(${unit}_mode ${unit})
    GN_debug("mode" "${${unit}_mode}")
    
    # create target
    if (${${unit}_mode} STREQUAL "app") 
        # create an executable
        GN_asApp(${unit})
    elseif(${${unit}_mode} STREQUAL "lib")
        # create a static libraty
        GN_asLib(${unit})
    elseif(${${unit}_mode} STREQUAL "heads") 
        # create a header only project
        GN_asHeades(${unit})
    else()
        message(FATAL_ERROR "unsupported mode '${${unit}_mode}'")
        endif()
    # add definitions
    add_definitions(${${unit}_defs})
    
    # call onSetup events
    GN_callEvent("onSetup" ${unit})

    # inject include paths
    GN_setupIncludes(${unit})

    # enable hierarchic filers
    set_target_properties(${unit} PROPERTIES FOLDER "${${unit}_category}")

    # define the unit
    GN_define(${unit})
    endfunction()
