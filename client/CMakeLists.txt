cmake_minimum_required(VERSION 3.1.2)
# Build configuration

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
add_executable( client  main.cpp tcpclient.cpp)
target_link_libraries(  client unixsockpass
                        ${CONAN_LIBS})
