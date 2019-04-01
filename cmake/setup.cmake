macro(APPEND key values)
	foreach(value ${values})
		set(${key} "${${key}} ${value}")
		endforeach()
	endmacro()

macro(SetupEvniroment)
	if(MSVC)
		# enable filters
		set_property(GLOBAL PROPERTY USE_FOLDERS ON)
		# Use dynamic libs
		APPEND(CMAKE_CXX_FLAGS_RELEASE /MT  /Z7)
		APPEND(CMAKE_CXX_FLAGS_DEBUG   /MTd /Ot)
		# Multiprocessor support
		APPEND(CMAKE_CXX_FLAGS /MP)
		# misc
		APPEND(CMAKE_CXX_FLAGS -D_WIN32_WINNT=0x0501)
	else()
		endif()
	# instalation path
	set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)
	# asio c++17 warnings...
	add_compile_definitions(_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
	endmacro()
