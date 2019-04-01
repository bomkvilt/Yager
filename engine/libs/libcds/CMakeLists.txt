
# download data library
GN_Download_project(root "libcds" "./loader.cmake")

# date module
GB_Module( libcds 
    Public  "${root}/install/include"
    Mode    "headers"
    Libs    "debug;cds_d;optimized;cds"
    bFlat   on)
