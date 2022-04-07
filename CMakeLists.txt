cmake_minimum_required(VERSION 3.10)
project(Epsilon_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(EPSILON_DIR "${PROJECT_SOURCE_DIR}/core")
set(INCLUDES_DIR "${EPSILON_DIR}")
set(HEADER_FILES "${INCLUDES_DIR}/pch.hpp")

set(GLAD_DIR "${PROJECT_SOURCE_DIR}/thirdparty/glad")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")

option(DEBUG "Include debug symbols in the executable" OFF)

include_directories(${INCLUDES_DIR})
add_subdirectory(${INCLUDES_DIR})

include_directories(${GLAD_INCLUDE_DIR})

add_executable (Epsilon_Engine.exe "${EPSILON_DIR}/main.cpp" ${HEADER_FILES})

target_include_directories(Epsilon_Engine.exe PRIVATE "${INCLUDES_DIR}")
target_link_libraries(Epsilon_Engine.exe "${GLAD_DIR}/lib/libglad.a")
