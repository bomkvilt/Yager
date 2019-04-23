cmake_minimum_required(VERSION 3.0)
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

set(GN_bDebug off)
set(GN_bTests on )
set(GN_bUnity off)
SetupEvniroment()
GN_Solutoin("Grove")

#~~~~~~~~~~~~~~~~~~~~~~~| >>  |
GN_Subprojects(
    engine
    snake
    )
#~~~~~~~~~~~~~~~~~~~~~~~| >>  |
GN_Configure()
