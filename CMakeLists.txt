cmake_minimum_required(VERSION 3.10)

project(Epsilon-Engine)

set(CMAKE_CXX_STANDARD 11)

include_directories( ${MY_SOURCE_DIR}/src )
include_directories( ${MY_SOURCE_DIR}/include )

add_executable (Epsilon Engine main.cpp)

find_package (GLFW COMPONENTS REQUIRED deppend/glfw-3.2.bin.WIN32/lib-mingw/ )