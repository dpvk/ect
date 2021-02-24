cmake_minimum_required(VERSION 3.1.2)
project(unixconnpass CXX)

# Build configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
add_compile_options( 
		    #-g 
 		    -O2
                     -Wall
                     -Wextra
                     -Wpedantic
                     -Wunused-parameter
                    )


# Conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
set(CMAKE_VERBOSE_MAKEFILE ON)
#add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)  #conan packages problem fix.

message(STATUS "Conan libs: ${CONAN_LIBS}")


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib )
add_subdirectory( lib )
add_subdirectory( rev1 )
add_subdirectory( rev2 )
add_subdirectory( client )
