## --------------------------| variables |-------------------------- ##
## -----------| settings
set(GN_vcpkg_bEnabled     on)

## cvpkgRoot is a path to vcpkg directory
# NOTE: if it's relative - ${CMAKE_BINARY_DIR} relative
# NOTE: if it's absolute - the path will be used
# NOTE: if it's "" - VCPKG_ROOT env will be used instead
set(GN_vcpkg_vcpkgRoot    "vcpkg") 

## --------------------------| initialisation |-------------------------- ##

function(GN_vcpkg_init)
    # download a --vcpkg
    if (NOT GN_vcpkg_downloaded)
        # download and setup
        GN_vcpkg_download()
        # prevent next execution
        set(GN_vcpkg_downloaded true CACHE BOOL "" FORCE)
        endif()
    endfunction()

## --------------------------| interface |-------------------------- ##

function(GN_vcpkg_install name)
    set(flag GN_vcpkg_installed_${name})
    if (NOT ${flag})
        # install the package
        GN_info("installing package" "${name}")
        execute_process(COMMAND "${GN_vcpkg_vcpkgExec}" install "${name}"
            WORKING_DIRECTORY   "${GN_vcpkg_vcpkgRoot}"
            RESULTS_VARIABLE    result
            )
        if (result)
            GN_error("vcpkg result: ${result}")
            endif()
        GN_info("package installed: ${name}")

        # prevent next installattion
        set(${flag} true CACHE BOOL "" FORCE)
        endif()
    endfunction()

## --------------------------| internal |-------------------------- ##

function(GN_vcpkg_download)
    if(${GN_vcpkg_vcpkgRoot} STREQUAL "")
        # use global variable
        set(GN_vcpkg_vcpkgRoot $ENV{VCPKG_ROOT} CACHE STRING "")
    elseif(NOT IS_ABSOLUTE ${GN_vcpkg_vcpkgRoot})
        # generate an absolute path
        set(GN_vcpkg_vcpkgRoot ${CMAKE_BINARY_DIR}/${GN_vcpkg_vcpkgRoot} CACHE STRING "")
        endif()
    
    GN_infoLine()
    GN_info("" "downloading vcpkg...")
    GN_infoLine()
    GN_info("vcpkgroot" "${GN_vcpkg_vcpkgRoot}")

    # clone vcpkg from github. stay on master
    if(NOT EXISTS ${GN_vcpkg_vcpkgRoot})
        GN_info("status..." "cloning vcpkg in ${GN_vcpkg_vcpkgRoot}")
        execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${GN_vcpkg_vcpkgRoot})
        endif()

    # check if success
    if(NOT EXISTS ${GN_vcpkg_vcpkgRoot}/README.md)
        GN_error("cannot clone vcpkg to ${GN_vcpkg_vcpkgRoot}")
        endif()

    # init vcpkg
    if(WIN32)
        set(VCPKG_EXEC ${GN_vcpkg_vcpkgRoot}/vcpkg.exe)
        set(VCPKG_BOOTSTRAP ${GN_vcpkg_vcpkgRoot}/bootstrap-vcpkg.bat)
    else()
        set(VCPKG_EXEC ${GN_vcpkg_vcpkgRoot}/vcpkg)
        set(VCPKG_BOOTSTRAP ${GN_vcpkg_vcpkgRoot}/bootstrap-vcpkg.sh)
        endif()

    # boostrap vcpkg
    if(NOT EXISTS ${VCPKG_EXEC})
        GN_info("status..." "bootstrapping vcpkg in ${GN_vcpkg_vcpkgRoot}")
        execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${GN_vcpkg_vcpkgRoot})
        endif()

    # check if success
    if(NOT EXISTS ${VCPKG_EXEC})
        GN_error("cannot bootstrap vcpkg in ${GN_vcpkg_vcpkgRoot}")
        endif()

    # set custom toolchain
    set(CMAKE_TOOLCHAIN_FILE "${GN_vcpkg_vcpkgRoot}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    
    set(GN_vcpkg_vcpkgExec "${VCPKG_EXEC}" CACHE STRING "")
    GN_info("status..." "vcpkg installed in ${GN_vcpkg_vcpkgRoot}")
    endfunction()
