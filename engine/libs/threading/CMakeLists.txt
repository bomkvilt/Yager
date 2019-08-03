GN_vcpkg_install(concurrentqueue)

find_package(unofficial-concurrentqueue CONFIG REQUIRED)

add_library(imp-concurrentqueue INTERFACE IMPORTED GLOBAL)
target_link_libraries(imp-concurrentqueue INTERFACE unofficial::concurrentqueue::concurrentqueue)

GN_Unit(threading
    Units   reflect
    Libs    imp-concurrentqueue
)
