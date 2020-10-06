if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
	set(cppsocket_library_dependencies pthread)
endif()


if(WIN32)
	# Enable auto export feature
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
	set(BUILD_SHARED_LIBS TRUE)

	# Change the default installation path
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		get_filename_component(INSTALL_PATH_COMPONENT ${CMAKE_INSTALL_PREFIX} PATH)
		set(CMAKE_INSTALL_PREFIX ${INSTALL_PATH_COMPONENT}/${cppsocket_namespace} CACHE PATH "" FORCE)
	endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

    set(cppsocket_library_dependencies ws2_32)
endif()
