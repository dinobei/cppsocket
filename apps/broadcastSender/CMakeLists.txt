include_directories(${cppsocket_include_directories})
add_executable(appBroadcastSender main.cpp)
target_link_libraries (appBroadcastSender ${the_library})
