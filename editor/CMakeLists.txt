cmake_minimum_required(VERSION 3.5.0)
project(Editor VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)


set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")

include_directories(${CMAKE_SOURCE_DIR} ../)
include_directories(${THIRDPARTY_DIR}/stb)

set_source_files_properties( ${PROJECT_ICON} PROPERTIES RESOURCES Resources )

add_executable(${PROJECT_NAME} main.cpp  ${PROJECT_ICON})

target_sources(${PROJECT_NAME} PRIVATE main.cpp editor.cpp)

target_precompile_headers(${PROJECT_NAME} REUSE_FROM Epsilon)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
add_dependencies(${PROJECT_NAME} Epsilon)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fsized-deallocation -g")

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    target_link_libraries(${PROJECT_NAME} PUBLIC Epsilon)

    else ()
    link_libraries(${PROJECT_NAME} PRIVATE ${EPSILON_BUILD_LOCATION}/libEpsilon.so)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3 Epsilon ${Vulkan_LIBRARIES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenAL32.dll)
    target_link_libraries(${PROJECT_NAME} PUBLIC shlwapi)
    target_link_libraries(${PROJECT_NAME} PUBLIC comctl32)
    target_link_libraries(${PROJECT_NAME} PUBLIC propsys)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${THIRDPARTY_DIR}/nethost.lib  OleAut32)
    
else()
    set(NetHostDir "~/.dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/8.0.4/runtimes/linux-x64/native")
    target_link_directories(${PROJECT_NAME} PRIVATE ${NetHostDir})
    target_link_libraries(${PROJECT_NAME} PRIVATE openal)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    target_link_libraries(${PROJECT_NAME} PRIVATE nethost)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
    
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,$ORIGIN,--disable-new-dtags")
endif()

    #target_link_libraries(${PROJECT_NAME} PRIVATE zlibstatic)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
include(InstallRequiredSystemLibraries)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
