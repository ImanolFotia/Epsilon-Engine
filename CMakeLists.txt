cmake_minimum_required(VERSION 3.0.0)
project(LearningVulkan VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)
#include_directories(/c/VulkanSDK/1.2.182.0/Include)

#link_directories(/c/VulkanSDK/1.2.182.0/Lib)
#link_directories(/c/VulkanSDK/1.2.182.0/Bin)



set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

include(ExternalProject)

#Download and include glm
ExternalProject_Add(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)

ExternalProject_Get_property(glm SOURCE_DIR)
set(GLM_BUILD_DIR ${SOURCE_DIR})
message(${GLM_BUILD_DIR})

include_directories(${GLM_BUILD_DIR})

ExternalProject_Add(glfw3_d
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)

ExternalProject_Get_property(glfw3_d SOURCE_DIR)
ExternalProject_Get_property(glfw3_d BINARY_DIR)
set(GLFW3_BUILD_DIR ${BINARY_DIR})
message(${SOURCE_DIR})
message(${GLFW3_BUILD_DIR})

include_directories(${SOURCE_DIR}/include)
link_directories(${GLFW3_BUILD_DIR}/src)


add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION ${GLFW3_BUILD_DIR}/src/libglfw3.a)
add_dependencies(glfw3 hidapi_external)

SET(HEADERS
    ${CMAKE_SOURCE_DIR}/framework
    ${CMAKE_SOURCE_DIR}/vk
)

add_executable(LearningVulkan main.cpp)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} glfw3)
add_dependencies(${PROJECT_NAME} glfw3_d)
add_dependencies(${PROJECT_NAME} glm)



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
