cmake_minimum_required(VERSION 3.5.0)
project(BSP VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ../../)

set_source_files_properties( ${PROJECT_ICON} PROPERTIES RESOU Resources )
add_executable(${PROJECT_NAME} main.cpp bsp_definitions.hpp SourceBSP.hpp  ${PROJECT_ICON})

target_sources(${PROJECT_NAME} PRIVATE main.cpp BSP.hpp)

message(STATUS "location of epsilon build: " ${EPSILON_BUILD_LOCATION})
message(STATUS "location of vulkan libs: " ${Vulkan_LIBRARIES})

add_dependencies(${PROJECT_NAME} Epsilon)
set(BUILD_AS_LIBRARY ON)

if (TRUE)
if(WIN32)
    if (MSVC)
        target_link_libraries(${PROJECT_NAME} PUBLIC Epsilon)
    else ()
        target_link_libraries(${PROJECT_NAME} PUBLIC Epsilon)
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

if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE openal32)
#target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
else()
target_link_libraries(${PROJECT_NAME} PRIVATE openal)
#target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})

set(CMAKE_INSTALL_DEBUG_LIBRARIES ON)


set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
include(InstallRequiredSystemLibraries)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)



