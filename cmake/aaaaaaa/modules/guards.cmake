## --------------------------| variables |-------------------------- ##
## -----------| settings
set(GN_guards_bEnabled      on      CACHE BOOL "whether the unit is enabled")
set(GN_guards_extentions    "h;hpp" CACHE STRING "list of supported extentions")

## --------------------------| internal |-------------------------- ##

function(GN_guards_processHeaders files)
    # if disables
    if (NOT GN_guards_bEnabled)
        return()
        endif()

    # make a mutch expression
    string(REPLACE "." "\\." clean ${GN_guards_extentions})
    list(JOIN clean "|" exts)

    # process headers
    foreach(file ${files})
        if (file MATCHES "${exts}")
            GN_guards_processHeader(${file})
            endif()
        endforeach()
    endfunction()

function(GN_guards_processHeader file)
    # generate guards
    GN_guards_guardName(name ${file})
    GN_guards_topGuard(top ${name})
    GN_guards_botGuard(bot ${name})

    # check guards
    file(READ ${file} data)
    GN_guards_checkGuards(ok "${data}")

    # insert guards
    set(out "")
    if (ok)
        string(APPEND re1 "^")
        string(APPEND re1 "#ifndef [A-Za-z0-9_]*\r?\n")
        string(APPEND re1 "#define [A-Za-z0-9_]*\r?\n")
        string(REGEX REPLACE "${re1}" "${top}" out "${data}")

        string(APPEND re2 "#endif *(//)?[!/ ]*[A-Za-z0-9_]*[\r\n ]*")
        string(REGEX REPLACE "${re2}" "${bot}" out "${out}")
    else()
        string(APPEND out "${top}")
        string(APPEND out "${data}")
        string(APPEND out "${bot}")
        endif()
    file(WRITE ${file} "${out}")
    endfunction()

function(GN_guards_guardName _result file)
    get_filename_component(name ${file} NAME)
    string(REPLACE "." "_" _d ${name})
    string(TOUPPER ${_d} _upper)
    GN_return(${_upper})
    endfunction()

function(GN_guards_topGuard _result name)
    set(_gt "")
    string(APPEND _gt "#ifndef ${name}\n")
    string(APPEND _gt "#define ${name}\n")
    GN_return(${_gt})
    endfunction()

function(GN_guards_botGuard _result name)
    GN_return("#endif //!${name}\n")
    endfunction()

function(GN_guards_checkGuards _result data)
    set(re "")
    string(APPEND re "^")
    string(APPEND re "#ifndef [A-Za-z0-9_]*\r?\n")
    string(APPEND re "#define [A-Za-z0-9_]*\r?\n")
    string(REGEX MATCH "${re}" _matched "${data}")
    GN_return("${_matched}" STREQUAL "")
    endfunction()
