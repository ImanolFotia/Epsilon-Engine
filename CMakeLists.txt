cmake_minimum_required(VERSION 3.0.0)
project(LearningVulkan VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

include(ExternalProject)

#Download and include glm
ExternalProject_Add(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    UPDATE_COMMAND ""
)

ExternalProject_Get_property(glm SOURCE_DIR)
set(GLM_BUILD_DIR ${SOURCE_DIR})
message(${GLM_BUILD_DIR})

include_directories(${GLM_BUILD_DIR})

ExternalProject_Add(glfw3_d
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    UPDATE_COMMAND ""
)

ExternalProject_Get_property(glfw3_d SOURCE_DIR)
ExternalProject_Get_property(glfw3_d BINARY_DIR)
set(GLFW3_BUILD_DIR ${BINARY_DIR})
message(${SOURCE_DIR})
message(${GLFW3_BUILD_DIR})

include_directories(${SOURCE_DIR}/include)
link_directories(${GLFW3_BUILD_DIR}/src)

SET(HEADERS 
    ${HEADERS}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/framework
    ${CMAKE_SOURCE_DIR}/vk
    ${CMAKE_SOURCE_DIR}/engine
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/renderers/vulkan.cpp)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS}) 
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT}) 
add_dependencies(${PROJECT_NAME} glfw3_d)
add_dependencies(${PROJECT_NAME} glm)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
