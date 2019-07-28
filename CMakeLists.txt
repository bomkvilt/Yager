cmake_minimum_required(VERSION 3.14)

#~~~~~~~~~~~~~~~~~~~~~~~| >>  |
if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
    endif()
if (POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
    endif()
#~~~~~~~~~~~~~~~~~~~~~~~| >>  |

include("cmake/setup.cmake")
include("cmake/gremlin/gremlin.cmake")

GN_option(GN_bDebug off)
GN_option(GN_bTests on)
GN_option(GN_vcpkg_vcpkgRoot "")

GN_Init()
project("Engine")
SetupEvniroment()
GN_Subprojects(
    engine
    snake
)
