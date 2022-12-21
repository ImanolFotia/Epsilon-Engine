cmake_minimum_required(VERSION 3.0.0)
project(Chess VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ../../)


add_executable(${PROJECT_NAME} chess.hpp main.cpp)

target_sources(${PROJECT_NAME} PRIVATE main.cpp )
target_sources(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/framework/env.cpp
        ${CMAKE_SOURCE_DIR}/engine/renderer/vulkan/vulkan_impl.cpp
        ${CMAKE_SOURCE_DIR}/engine/renderer/vulkan/vulkan.cpp
        ${CMAKE_SOURCE_DIR}/engine/renderer/vulkan/resource_manager.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})



