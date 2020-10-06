cmake_minimum_required (VERSION 3.4)
set(the_library cppsocket)
set (cppsocket_namespace cppsocket)
project (${the_library})

set (CMAKE_CXX_STANDARD 11)

include(cmake/Utils.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(cppsocket_include_directories)

include(cmake/PlatformSpecific.cmake)

option (cppsocket_BUILD_SHARED "Build shared library" FALSE)
if(cppsocket_BUILD_SHARED)
    set(the_library_type SHARED)
    if(MSVC)
        msvc_multi_threaded_static_turn(OFF)
    endif()
else()
    set(the_library_type STATIC)
    if(MSVC)
        msvc_multi_threaded_static_turn(ON)
    endif()
endif()

add_subdirectory(source)

option (cppsocket_BUILD_APPS "Build example apps" TRUE)
if(cppsocket_BUILD_APPS)
    add_subdirectory(apps)
endif()
