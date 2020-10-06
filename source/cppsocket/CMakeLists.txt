file(GLOB_RECURSE local_header_files *.h)
file(GLOB_RECURSE local_source_files *.cpp)

list(APPEND cppsocket_source_files ${local_source_files})
list(APPEND cppsocket_header_files ${local_header_files})

set(cppsocket_source_files ${cppsocket_source_files} PARENT_SCOPE)
set(cppsocket_header_files ${cppsocket_header_files} PARENT_SCOPE)
