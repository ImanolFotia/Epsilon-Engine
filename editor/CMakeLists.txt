cmake_minimum_required(VERSION 3.0.0)
project(Editor VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)


set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")

include_directories(${CMAKE_SOURCE_DIR} ../)

set_source_files_properties( ${PROJECT_ICON} PROPERTIES RESOURCES Resources )

add_executable(${PROJECT_NAME} main.cpp editor.cpp ${PROJECT_ICON})

target_sources(${PROJECT_NAME} PRIVATE main.cpp editor.cpp)

add_dependencies(${PROJECT_NAME} Epsilon)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    target_link_libraries(${PROJECT_NAME} PUBLIC Epsilon)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC Epsilon)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3 Epsilon ${THIRDPARTY_DIR}/nethost.lib  OleAut32 ${CMAKE_DL_LIBS} ${Mono_LINK_LIBRARIES} ${Vulkan_LIBRARIES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenAL32.dll)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE openal)
endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE zlibstatic)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
include(InstallRequiredSystemLibraries)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)