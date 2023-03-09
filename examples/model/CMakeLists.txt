cmake_minimum_required(VERSION 3.0.0)
project(Text VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ../../)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

message(STATUS "location of epsilon build: " ${EPSILON_BUILD_LOCATION})
message(STATUS "location of vulkan libs: " ${Vulkan_LIBRARIES})

add_dependencies(${PROJECT_NAME} Epsilon)

if (BUILD_AS_LIBRARY)
if(WIN32)
    if (MSVC)
        target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Epsilon)
    else ()
        target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Epsilon)
    endif()
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${EPSILON_BUILD_LOCATION}/libEpsilon.so)
    #  target_link_libraries(${PROJECT_NAME} PUBLIC Epsilon)
endif ()
else()
    target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_FILES})
endif()


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)

target_link_libraries(${PROJECT_NAME} PRIVATE ${Mono_LINK_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Mono_INCLUDE_DIRS})

if(DBUILD_ANDROID EQUAL 0)
if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE openal32)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp-5.dll)
else()
target_link_libraries(${PROJECT_NAME} PRIVATE openal)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
endif()
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})



