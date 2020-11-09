include_directories(${cppsocket_include_directories})
add_executable(appBroadcastReceiver main.cpp)
target_link_libraries (appBroadcastReceiver ${the_library})
