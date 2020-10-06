include_directories(${cppsocket_include_directories})
add_executable(appEchoServer main.cpp)
target_link_libraries (appEchoServer ${the_library})
