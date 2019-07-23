## --------------------------| variables |-------------------------- ##
## -----------| settings
set(GN_test_bEnabled   on)

## --------------------------| initialisation |-------------------------- ##

function(GN_test_init)
    # download gtest
    GN_test_download()
    endfunction()

## --------------------------| internal |-------------------------- ##

function(GN_test_Add unit)
    # TODO:: add a test target
    endfunction()

function(GN_test_download)
    GN_vcpkg_install("gtest")
    endfunction()
