set(cppsocket_source_files)
set(cppsocket_header_files)

add_subdirectory(cppsocket)

file(GLOB_RECURSE files "*.*")
cppsocket_group_files("${files}")

include(${PROJECT_SOURCE_DIR}/cmake/Version.cmake)

list(APPEND cppsocket_include_directories ${CMAKE_CURRENT_LIST_DIR})
set(cppsocket_include_directories ${cppsocket_include_directories} PARENT_SCOPE)

include_directories(${cppsocket_include_directories})
message("@@@: " ${cppsocket_include_directories})
add_library(${the_library} ${the_library_type} ${cppsocket_source_files} ${cppsocket_header_files})
target_link_libraries(${the_library} ${cppsocket_library_dependencies})

# include(${PROJECT_SOURCE_DIR}/cmake/Install.cmake)

# set_property(TARGET ${the_library} PROPERTY VERSION ${cppsocket_version})
# set_property(TARGET ${the_library} PROPERTY SOVERSION 1)
# set_property(TARGET ${the_library} PROPERTY
#   INTERFACE_${the_library}_MAJOR_VERSION 0)
# set_property(TARGET ${the_library} APPEND PROPERTY
#   COMPATIBLE_INTERFACE_STRING ${the_library}_MAJOR_VERSION
# )
# set_property(TARGET ${the_library} PROPERTY INTERFACE_INCLUDE_DIRECTORIES
# 			${CMAKE_INSTALL_PREFIX}/include)
# if(WIN32)
# 	set_target_properties(${the_library} PROPERTIES COMPILE_FLAGS "/wd4819")
# endif()
