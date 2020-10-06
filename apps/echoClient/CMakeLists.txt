include_directories(${cppsocket_include_directories})
add_executable(appEchoClient main.cpp)
target_link_libraries (appEchoClient ${the_library})
