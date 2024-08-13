cmake_minimum_required(VERSION 3.0.0)
project(BasicExample VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} basic.cpp)
#add_library(${PROJECT_NAME} SHARED main_lib.cpp)
#if(MSVC)
#  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
#else()
#  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
#endif()

#link_directories(${CMAKE_BINARY_DIR})
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE LearningVulkan)
#target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS}) 


#add_dependencies(${PROJECT_NAME} glfw3_d)
#add_dependencies(${PROJECT_NAME} glm)
#add_dependencies(${PROJECT_NAME} stb)
#add_dependencies(${PROJECT_NAME} LearningVulkan)

