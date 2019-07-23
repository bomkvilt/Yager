## --------------------------| helpers |-------------------------- ##
## -----------| common

# project creates a new cmake project
macro(GN_project name)
    project(${name})
    set(CMAKE_CXX_STANDARD ${GN_cpp_version})
    endmacro()

# return returns the value from a function to a '_result'
# NOTE: '_result' containse a name of a parent scope's variable
macro(GN_return value)
    set(${_result} "${value}" PARENT_SCOPE)
    return()
    endmacro()

# default sets a 'value' to _result if it's not presented
function(GN_default _result value)
    if ("${${_result}}" STREQUAL "")
        GN_return("${value}")
        endif()
    endfunction()

## -----------| global storage

# set sets the key to a global storage
function(GN_set name value)
    set_property(GLOBAL PROPERTY ${name} ${value})
    endfunction()

# get gets a key from a global storage
function(GN_get _result name)
    get_property(out GLOBAL PROPERTY ${name})
    GN_return("${out}")
    endfunction()

# getList gets the names from a global storage
# NOTE: bUnique makes output elements unique
# NOTE: ${argn} will be uppended to the list
function(GN_getList _result names member bUnique)
    # get elements
    # NOTE: "" is required to avoid REMOVE_DUPLICATES' error
    set(array "")
    foreach(elem ${names})
        GN_get(subs ${elem}${member})
        list(APPEND array ${subs})
        endforeach()
    # uppend taken elements
    list(APPEND array ${ARGN})
    # remove duplicates
    if (bUnique)
        list(REMOVE_DUPLICATES array)
        endif()
    # remove a tmp element
    list(REMOVE_ITEM array "")
    # done
    GN_return("${array}")
    endfunction()
