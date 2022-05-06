cmake_minimum_required(VERSION 3.0.0)
project(Epsilon-Engine VERSION 0.1.0)

include(CTest)
enable_testing()

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

include(ExternalProject)
#Download and include Glad
ExternalProject_Add(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."

    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/thirdparty/glad
)
ExternalProject_Get_property(glad BINARY_DIR)
set(GLAD_BUILD_DIR ${BINARY_DIR})

include_directories(${GLAD_BUILD_DIR}/include)

#Download and include glm
ExternalProject_Add(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."

    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/thirdparty/glm
)

ExternalProject_Get_property(glm SOURCE_DIR)
set(GLM_BUILD_DIR ${SOURCE_DIR})
message(${GLM_BUILD_DIR})

include_directories(${GLM_BUILD_DIR})

#Download and include glfw3  https://github.com/glfw/glfw.git

ExternalProject_Add(glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."

    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/thirdparty/glfw3
)

ExternalProject_Get_property(glfw3 SOURCE_DIR)
ExternalProject_Get_property(glfw3 BINARY_DIR)
set(GLFW3_BUILD_DIR ${BINARY_DIR})
message(${SOURCE_DIR})

include_directories(${SOURCE_DIR}/include)
link_directories(${GLFW3_BUILD_DIR}/src)

SET(HEADERS
    ${CMAKE_SOURCE_DIR}/core
)

include_directories( ${HEADERS} )

add_executable(Epsilon-Engine ${CMAKE_SOURCE_DIR}/core/main.cpp)
add_dependencies(Epsilon-Engine glad)

#target_link_libraries(Epsilon-Engine PRIVATE glad)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
