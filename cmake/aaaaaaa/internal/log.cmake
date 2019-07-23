## --------------------------| log |-------------------------- ##
## -----------| settings
set(GN_log_lvl  "DEBUG")
set(GN_log_key  20     )


## -----------| interface

# print a dubug message
# NOTE: if ARGN is presented mesage: msg=key, ARGN[0]=value
# NOTE: if it's required to print values as in n columns: ARGN[1]=n
function(GN_debug msg)
    GN_log_lvlBorder("DEBUG")
    GN_log_process(msg "${msg}" ${ARGN})
    message("${msg}")
    endfunction()

# print an info message
# NOTE: if ARGN is presented mesage: msg=key, ARGN[0]=value
# NOTE: if it's required to print values as in n columns: ARGN[1]=n
function(GN_info msg)
    GN_log_lvlBorder("INFO")
    GN_log_process(msg "${msg}" ${ARGN})
    message("${msg}")
    endfunction()

# print an error message
# NOTE: if ARGN is presented mesage: msg=key, ARGN[0]=value
# NOTE: if it's required to print values as in n columns: ARGN[1]=n
# NOTE: the function terminates cmake execution
function(GN_error msg)
    GN_log_lvlBorder("ERROR")
    GN_log_process(msg "${msg}" ${ARGN})
    message(FATAL_ERROR "${msg}")
    endfunction()


function(GN_debugLine)
    string(REPEAT "-" 80 line)
    GN_debug(${line})
    endfunction()

function(GN_infoLine)
    string(REPEAT "-" 80 line)
    GN_info(${line})
    endfunction()

function(GN_errorLine)
    string(REPEAT "-" 80 line)
    GN_error(${line})
    endfunction()

## -----------| internal

macro(GN_log_lvlBorder lvl)
    GN_log_lvlToInt(min ${GN_log_lvl})
    GN_log_lvlToInt(cur ${lvl})
    if (cur LESS min)
        return()
        endif()
    
    if (NOT GN_bDebug)
        GN_log_lvlToInt(min "DEBUG")
        if (NOT (cur GREATER min))
            return()
            endif()
        endif()
    endmacro()

function(GN_log_lvlToInt _result lvl)
    if    (${lvl} STREQUAL "DEBUG")
        GN_return(0)
    elseif(${lvl} STREQUAL "INFO")
        GN_return(1)
    elseif(${lvl} STREQUAL "ERROR")
        GN_return(2)
        endif()
    # wtf???
    message(FATAL_ERROR "incorrect log level '${lvl}'")
    endfunction()


function(GN_log_process _result msg)
    # just message
    if (${ARGC} EQUAL 2)
        GN_return("${msg}")
        endif()
    
    # key-value
    set(key "${msg}")
    list(GET ARGN 0 value)
    if (${ARGC} EQUAL 3)
        GN_log_kv(msg "${key}" "${value}" 1)
        GN_return("${msg}")
        endif()

    # key value with backet size
    list(GET ARGN 1 backet)
    if (${ARGC} EQUAL 4)
        GN_log_kv(msg "${key}" "${value}" "${backet}")
        GN_return("${msg}")
        endif()

    # wtf???
    message(FATAL_ERROR "incorrect count of arguments")
    endfunction()

# kv format key and value to a string
function(GN_log_kv _result key msg backet)
    GN_log_getKey(key "${key}")
    GN_log_getIdent(ident)

    # how many values we have
    list(LENGTH msg len)
    math(EXPR end "${len} - 1")
    if (${end} LESS 0)
        GN_return("")
        endif()

    set(bKeyUsed off)
    set(message "")
    # generate a message string
    foreach(i RANGE 0 ${end} ${backet})
        # get a sublist and ident it's elements
        list(SUBLIST msg ${i} ${backet} elements)
        list(JOIN elements "\t" row)
        string(APPEND message "${key} : ${row}")
        
        # change the key to ident
        if (NOT bKeyUsed)
            set(bKeyUsed on)
            set(key "\n${ident}")
            endif()
        endforeach()
    GN_return(${message})
    endfunction()


# getKey returns an idented key string
function(GN_log_getKey _result key)
    # append ' ' to key to ident
    string(LENGTH "${key}" len)
    if (len LESS ${GN_log_key})
        # how many ' ' we need to insert
        math(EXPR delta "${GN_log_key} - ${len}")
        # generate the string
        string(REPEAT " " ${delta} ident)
        string(APPEND key ${ident})
        GN_return(${key})
        endif()
    # trim input string
    string(SUBSTRING "${key}" 0 ${GN_log_key} key)
    GN_return(${key})
    endfunction()

# getIdent returns an ident string 
function(GN_log_getIdent _result)
    string(REPEAT " " ${GN_log_key} ident)
    GN_return(${key})
    endfunction()
