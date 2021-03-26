cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GLAD_DIR "${PROJECT_SOURCE_DIR}/thirdparty/glad")

option(DEBUG "Include debug symbols in the executable" OFF)

project(Epsilon-Engine)

include_directories( ${MY_SOURCE_DIR}/core )

add_executable (Epsilon_Engine.exe main.cpp)


add_library(GLAD)
target_link_libraries(GLAD "${GLAD_DIR}/lib/libglad.a")

find_package (GLAD COMPONENTS REQUIRED ${GLAD_DIR} )
find_package (BULLET3 COMPONENTS REQUIRED thirdparty/bullet3)
find_package (GLFW COMPONENTS REQUIRED thirdparty/glfw)
find_package (GLM COMPONENTS REQUIRED thirdparty/glm)
find_package (IMGUI COMPONENTS REQUIRED thirdparty/imgui-docking)
find_package (INIH COMPONENTS REQUIRED thirdparty/inih)
find_package (JSON-DEVELOP COMPONENTS REQUIRED thirdparty/json-develop)
find_package (LUA COMPONENTS REQUIRED thirdparty/lua5.3.5)
find_package (OPENAL COMPONENTS REQUIRED thirdparty/openal-soft)
find_package (SOIL COMPONENTS REQUIRED thirdparty/soil)
find_package (XAUDIO COMPONENTS REQUIRED thirdparty/XAudio2)