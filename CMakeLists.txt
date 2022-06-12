cmake_minimum_required(VERSION 3.0.0)
project(LearningVulkan VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set(DBUILD_SHARED_LIBS ON CACHE BOOL "Build as a shared library")

option (BUILD_AS_LIBRARY "Build as a library" OFF)

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


#Download and include vma
ExternalProject_Add(vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    UPDATE_COMMAND ""
)

ExternalProject_Get_property(vma SOURCE_DIR)
set(VMA_BUILD_DIR ${SOURCE_DIR})
message(${VMA_BUILD_DIR})

include_directories(${VMA_BUILD_DIR})


#Download and include stb-image
ExternalProject_Add(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    INSTALL_COMMAND cmake -E echo "Skipping install step for stb-image."
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
)

ExternalProject_Get_property(stb SOURCE_DIR)
set(STB_BUILD_DIR ${SOURCE_DIR})
message(${STB_BUILD_DIR})

include_directories(${STB_BUILD_DIR})

# download and include glfw3
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

#add_library(${PROJECT_NAME} SHARED main_lib.cpp)
#if(MSVC)
#  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
#else()
#  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
#endif()

option(CREATE_STATIC_LIB "Build static library" OFF)

if(BUILD_AS_LIBRARY)
    if(CREATE_STATIC_LIB)
        add_library(${PROJECT_NAME} STATIC main_lib.cpp)
    else()
        add_library(${PROJECT_NAME} SHARED main_lib.cpp)
    endif()
else()
    add_executable(${PROJECT_NAME} main.cpp)
endif()

target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/framework/env.cpp
    ${CMAKE_SOURCE_DIR}/engine/renderer/vulkan/vulkan_impl.cpp
    ${CMAKE_SOURCE_DIR}/engine/renderer/vulkan/vulkan.cpp)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS}) 

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

add_dependencies(${PROJECT_NAME} glfw3_d)
add_dependencies(${PROJECT_NAME} glm)
add_dependencies(${PROJECT_NAME} stb)
add_dependencies(${PROJECT_NAME} vma)
#add_dependencies(${PROJECT_NAME} LearningVulkan)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

execute_process (
    COMMAND bash -c ${CMAKE_SOURCE_DIR}/build_shaders.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE outVar
)

#execute_process(
#    COMMAND mkdir -p ${PROJECT_BINARY_DIR}/examples/assets/shaders/
#)

install(DIRECTORY "./assets/shaders/" 
        DESTINATION ${PROJECT_BINARY_DIR}/examples/assets/shaders/
        FILES_MATCHING PATTERN "*.spv" )


#add_subdirectory(examples/basic)