cmake_minimum_required(VERSION 3.1.2)
# Build configuration

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
add_executable( rev1  main.cpp)
target_link_libraries(  rev1 unixsockpass
                        ${CONAN_LIBS})
