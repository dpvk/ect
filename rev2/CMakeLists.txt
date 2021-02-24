cmake_minimum_required(VERSION 3.1.2)
# Build configuration

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
add_executable( rev2  main.cpp)
target_link_libraries(  rev2 unixsockpass
                        ${CONAN_LIBS})
