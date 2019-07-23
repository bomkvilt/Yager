## --------------------------| modules |-------------------------- ##
## -----------| common

macro(GN_callEvent event unit)
    foreach(module ${GN_modules_enabled})
        set(file "${GN_dir_gremlin}/modules/${module}.${event}.cmake")
        if (EXISTS ${file})
            include(${file})
            event(${unit})
            endif()
        endforeach()
    endmacro()
