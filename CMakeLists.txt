cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set(DBUILD_SHARED_LIBS ON CACHE BOOL "Build as a shared library")
set(DGENERATE_RPM_PACKAGE OFF CACHE BOOL "Generate RPM deployment package")
set(EPSILON_BUILD_LOCATION ${CMAKE_BINARY_DIR})
set(IS_CI_ENV FALSE CACHE BOOL "It's in a CI environment")

set(BUILD_ANDROID FALSE CACHE BOOL "Build for Android")
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS=ON)
set(DIMGUI_DEFINE_MATH_OPERATORS TRUE)
set(IMGUI_DEFINE_MATH_OPERATORS "Define math operators for imgui" ON)

add_compile_definitions(BUILD_SHARED_LIBS=${DBUILD_SHARED_LIBS})


set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
set(PROJECT_ICON "${CMAKE_SOURCE_DIR}/media/icon.rc")
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
include(InstallRequiredSystemLibraries)

add_compile_definitions(BUILD_ANDROID=${BUILD_ANDROID})

# use, i.e. don't skip the full RPATH for the build tree
set(SET_CUSTOM_RPATH TRUE)
set(DESTDIR ~)
set(CPACK_SET_DESTDIR ~/.local)
if(SET_CUSTOM_RPATH)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:../lib")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_BUILD_RPATH "${CMAKE_INSTALL_PREFIX}/lib:../lib")

    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH}")
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

endif()

option(BUILD_AS_LIBRARY "Build as a library" ON)
option(DOWNLOAD_LIBRARIES "Download thirdparty libraries" ON)
option(GFX_BACKEND_VULKAN "Use Vulkan as graphics backend" ON)
option(GFX_BACKEND_WEBGPU "Use WebGPU as graphics backend" OFF)
option(GFX_BACKEND_WEBGL "Use WebGL as graphics backend" OFF)

if(BUILD_AS_LIBRARY EQUAL ON)
    set(DBUILD_AS_LIBRARY 1)
    add_compile_definitions(DBUILD_AS_LIBRARY=TRUE)
endif()

set(DBUILD_AS_LIBRARY TRUE)
add_compile_definitions(BUILD_AS_LIBRARY=TRUE)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL=TRUE)


include(FindPkgConfig)


if(BUILD_ANDROID)

    set(DUSE_GLFW 0)
    add_compile_definitions(USE_GLFW=${DUSE_GLFW})
    set(CMAKE_INSTALL_PREFIX "install/arm64-v8a" CACHE STRING "Install prefix")
    set(CMAKE_TOOLCHAIN_FILE "J:\\AndroidSDK\\ndk\\26.0.10404224-beta1\\build\\cmake\\android.toolchain.cmake" CACHE STRING "Android toolchain file")
    set(ANDROID_PLATFORM "34" CACHE STRING "Target Platform")
    set(ANDROID_STL "c++_shared" CACHE STRING "Android STL")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
    set(ANDROID_CPP_FEATURES "rtti exceptions" CACHE STRING "CPP Features")
    set(ANDROID_ABI arm64-v8a CACHE STRING "Android ABI")
    set(ANDROID_USE_LEGACY_TOOLCHAIN_FILE ON CACHE BOOL "Use legacy toolchain")
else()

    set(DUSE_GLFW 1)
    add_compile_definitions(USE_GLFW=${DUSE_GLFW})
endif()

include(ExternalProject)
#Download and include glm
ExternalProject_Add(glm
    SOURCE_DIR ${THIRDPARTY_DIR}/glm
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

ExternalProject_Get_property(glm SOURCE_DIR)
set(GLM_BUILD_DIR ${SOURCE_DIR})
message(${GLM_BUILD_DIR})

include_directories(${GLM_BUILD_DIR})

#Download and include vma
ExternalProject_Add(vma
    SOURCE_DIR ${THIRDPARTY_DIR}/vma
    BINARY_DIR ${THIRDPARTY_DIR}/vma/build
    TMP_DIR ${THIRDPARTY_DIR}/vma/tmp
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    UPDATE_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

ExternalProject_Get_property(vma SOURCE_DIR)
set(VMA_BUILD_DIR ${SOURCE_DIR})
message(${VMA_BUILD_DIR})

include_directories(${VMA_BUILD_DIR}/include)

#Download and include stb-image
ExternalProject_Add(stb
    INSTALL_COMMAND cmake -E echo "Skipping install step for stb-image."

    SOURCE_DIR ${THIRDPARTY_DIR}/stb
    BINARY_DIR ${THIRDPARTY_DIR}/stb/build
    TMP_DIR ${THIRDPARTY_DIR}/stb/tmp
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

ExternalProject_Get_property(stb SOURCE_DIR)
set(STB_BUILD_DIR ${SOURCE_DIR})
message(${STB_BUILD_DIR})

include_directories(${STB_BUILD_DIR})

#Download and include Beacon
ExternalProject_Add(beacon
    SOURCE_DIR ${THIRDPARTY_DIR}/beacon
    BINARY_DIR ${THIRDPARTY_DIR}/beacon/build
    TMP_DIR ${THIRDPARTY_DIR}/beacon/tmp
    INSTALL_COMMAND cmake -E echo "Skipping install step for beacon."
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

ExternalProject_Get_property(beacon SOURCE_DIR)
set(BEACON_BUILD_DIR ${SOURCE_DIR})
message(${BEACON_BUILD_DIR})

include_directories(${BEACON_BUILD_DIR})

#Download and include nlohmann/tinygltf
ExternalProject_Add(tinygltf
    SOURCE_DIR ${THIRDPARTY_DIR}/tinygltf
    BINARY_DIR ${THIRDPARTY_DIR}/tinygltf/build
    TMP_DIR ${THIRDPARTY_DIR}/tinygltf/tmp
    INSTALL_COMMAND cmake -E echo "Skipping install step for json."
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    CONFIGURE_COMMAND ""
)

ExternalProject_Get_property(tinygltf SOURCE_DIR)
set(TINYGLTF_BUILD_DIR ${SOURCE_DIR})
message(${TINYGLTF_BUILD_DIR})

include_directories(${TINYGLTF_BUILD_DIR})

#Download and include nlohmann/json
ExternalProject_Add(json
    SOURCE_DIR ${THIRDPARTY_DIR}/json
    BINARY_DIR ${THIRDPARTY_DIR}/json/build
    TMP_DIR ${THIRDPARTY_DIR}/json/tmp
    INSTALL_COMMAND cmake -E echo "Skipping install step for json."
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    CONFIGURE_COMMAND ""
)


ExternalProject_Get_property(json SOURCE_DIR)
set(JSON_BUILD_DIR ${SOURCE_DIR})
message(${JSON_BUILD_DIR})

include_directories(${JSON_BUILD_DIR}/include)


install(FILES ${files}

    DESTINATION lib
)


#install( assimp_d LIBRARY DESTINATION lib)

##

# download and include glfw3
if(DUSE_GLFW)
    ExternalProject_Add(glfw3_d
        SOURCE_DIR ${THIRDPARTY_DIR}/glfw3
        BINARY_DIR ${THIRDPARTY_DIR}/glfw3/build
        TMP_DIR ${THIRDPARTY_DIR}/glfw3/tmp
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        UPDATE_COMMAND ""
    )

    ExternalProject_Get_property(glfw3_d SOURCE_DIR)
    ExternalProject_Get_property(glfw3_d BINARY_DIR)
    set(GLFW3_BUILD_DIR ${BINARY_DIR})
    message(${SOURCE_DIR})
    message(${GLFW3_BUILD_DIR})

    include_directories(${SOURCE_DIR}/include)
    link_directories(${GLFW3_BUILD_DIR}/src)
endif()

set(CMAKE_OPENAL_ARGS "")
if(WIN32)
    set(CMAKE_OPENAL_ARGS "")
else()
    if(BUILD_ANDROID)
        set(CMAKE_OPENAL_ARGS "-DALSOFT_REQUIRE_PULSEAUDIO=ON -DALSOFT_REQUIRE_PIPEWIRE=ON -DCMAKE_INSTALL_PREFIX=install/arm64-v8a -DCMAKE_TOOLCHAIN_FILE=\"J:\\AndroidSDK\\ndk\\26.0.10404224-beta1\\build\\cmake\\android.toolchain.cmake\" -DANDROID_PLATFORM=34 -DANDROID_STL=c++_shared -DANDROID_CPP_FEATURES=\"rtti exceptions\" -DANDROID_ABI=arm64-v8a
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
    else()
        set(CMAKE_OPENAL_ARGS "-DALSOFT_BACKEND_PULSEAUDIO=ON -DALSOFT_REQUIRE_PULSEAUDIO=ON -DALSOFT_REQUIRE_PIPEWIRE=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DALSOFT_BACKEND_PIPEWIRE=ON")
    endif()
endif()

# download and include openal
#requires installation of pulse audio and pipewire developer libs in the OS
ExternalProject_Add(openal_d
    SOURCE_DIR ${THIRDPARTY_DIR}/openal
    BINARY_DIR ${THIRDPARTY_DIR}/openal/build
    TMP_DIR ${THIRDPARTY_DIR}/openal/tmp
    CMAKE_ARGS ${CMAKE_OPENAL_ARGS}
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    UPDATE_COMMAND ""
)

ExternalProject_Get_property(openal_d SOURCE_DIR)
ExternalProject_Get_property(openal_d BINARY_DIR)
set(OPENAL_BUILD_DIR ${BINARY_DIR})
message(${SOURCE_DIR})
message(${OPENAL_BUILD_DIR})

include_directories(${SOURCE_DIR}/include)
link_directories(${OPENAL_BUILD_DIR})

ExternalProject_Add(zlib_d
    SOURCE_DIR ${THIRDPARTY_DIR}/zlib
    BINARY_DIR ${THIRDPARTY_DIR}/zlib/build
    TMP_DIR ${THIRDPARTY_DIR}/zlib/tmp
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    UPDATE_COMMAND ""
)

ExternalProject_Get_property(zlib_d SOURCE_DIR)
ExternalProject_Get_property(zlib_d BINARY_DIR)
set(ZLIB_DIR ${SOURCE_DIR})
set(ZLIB_BIN_DIR ${BINARY_DIR})
message(${ZLIB_BIN_DIR})
message(${ZLIB_DIR})

include_directories(${ZLIB_DIR})
include_directories(${ZLIB_DIR}/build)
link_directories(${ZLIB_DIR}/build)


set(ZLIB_INCLUDES "${ZLIB_DIR}/build")
message(${ZLIB_INCLUDES})
set(ZLIB_NAME "zlib.dll")
set(ZLIBSTATIC_NAME "zlibstatic.lib")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(ZLIB_NAME "zlibd.dll")
    set(ZLIBSTATIC_NAME "zlibstaticd.lib")
endif()
message("${ZLIB_DIR}/build/${ZLIB_NAME}")
if(WIN32)
    add_library(ZLIB SHARED IMPORTED)
    set_target_properties(ZLIB PROPERTIES
        IMPORTED_LOCATION "${ZLIB_DIR}/build/${ZLIB_NAME}"
        IMPORTED_IMPLIB "${ZLIB_DIR}/build/${ZLIBSTATIC_NAME}"
        INTERFACE_INCLUDE_DIRECTORIES ${ZLIB_INCLUDES}
    )
endif()

set(IMGUI_BUILD_DIR ${THIRDPARTY_DIR}/imgui)
message(${IMGUI_BUILD_DIR})

include_directories(${IMGUI_BUILD_DIR})
link_directories(${IMGUI_BUILD_DIR})


set(IMPLOT_BUILD_DIR ${THIRDPARTY_DIR}/implot)
message(${IMPLOT_BUILD_DIR})

include_directories(${IMPLOT_BUILD_DIR})
link_directories(${IMPLOT_BUILD_DIR})


set(IMGUIZMO_BUILD_DIR ${THIRDPARTY_DIR}/ImGuizmo)
set(ImGuizmo_DIR ${THIRDPARTY_DIR}/ImGuizmo)
message(${IMGUIZMO_BUILD_DIR})

include_directories(${IMGUIZMO_BUILD_DIR})
link_directories(${IMGUIZMO_BUILD_DIR})


set(TINYGLTF_BUILD_DIR ${THIRDPARTY_DIR}/tinygltf)
message(${TINYGLTF_BUILD_DIR})
include_directories(${TINYGLTF_BUILD_DIR})


include_directories(${THIRDPARTY_DIR}/FontAwesome)
set(HEADERS
    ${HEADERS}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/framework
    ${CMAKE_SOURCE_DIR}/vk
    ${CMAKE_SOURCE_DIR}/engine
)

include_directories(${CMAKE_SOURCE_DIR})

set(EPSILON_SOURCE_LOCATION ${CMAKE_SOURCE_DIR})

option(CREATE_STATIC_LIB "Build static library" OFF)

if(D__ANDROID__)
    set(CREATE_STATIC_LIB ON)
    add_compile_definitions(BUILD_AS_LIBRARY=1)
    set(BUILD_AS_LIBRARY ON)
endif()

project(Epsilon VERSION 0.1.0)

set(PROJECT_FILES
    ${CMAKE_SOURCE_DIR}/core/engine/context.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/env.cpp
    ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/vulkan_impl.cpp
    ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/vulkan.cpp
    ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/imgui/imgui_setup.cpp
    ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/resource_manager.cpp
    ${CMAKE_SOURCE_DIR}/core/engine/scene/structs/frustum.cpp
    ${CMAKE_SOURCE_DIR}/core/engine/audio/al/audio_manager.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/IO.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyBoard.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyBoard.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/DragDrop.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyboardWrapper.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/Mouse.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/Joystick.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/JoystickWrapper.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/Mappings.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/IO/IO.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/utils/helpers/camera.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/utils/helpers/camera.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/loaders/DDS.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/vfs/cifstream.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/vfs/zip.cpp
    ${CMAKE_SOURCE_DIR}/core/engine/scene/asset_manager.cpp
    ${CMAKE_SOURCE_DIR}/apis/al/listener.hpp
    ${CMAKE_SOURCE_DIR}/apis/al/source.hpp
    ${CMAKE_SOURCE_DIR}/apis/al/al_data.hpp
    ${CMAKE_SOURCE_DIR}/apis/al/device.hpp
    ${CMAKE_SOURCE_DIR}/apis/al/listener.hpp
    ${CMAKE_SOURCE_DIR}/core/framework/audio/WAVFile.h
    ${CMAKE_SOURCE_DIR}/core/framework/window.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/window.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/clock.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/loaders/model.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/log.cpp
    ${CMAKE_SOURCE_DIR}/Epsilon.hpp
    ${CMAKE_SOURCE_DIR}/Epsilon.cpp
    ${CMAKE_SOURCE_DIR}/core/framework/singleton.hpp
    ${CMAKE_SOURCE_DIR}/modules/imgui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/modules/imgui/imgui_impl_vulkan.cpp
    ${CMAKE_SOURCE_DIR}/core/ui/UI.cpp
    ${THIRDPARTY_DIR}/implot/implot_items.cpp
    ${THIRDPARTY_DIR}/implot/implot.cpp
    ${IMGUI_BUILD_DIR}/imgui.cpp
    ${IMGUI_BUILD_DIR}/imgui_demo.cpp
    ${IMGUI_BUILD_DIR}/imgui_draw.cpp
    ${IMGUI_BUILD_DIR}/imgui_tables.cpp
    ${IMGUI_BUILD_DIR}/imgui_widgets.cpp
    ${THIRDPARTY_DIR}/ImGuizmo/ImGuizmo.cpp
    ${TINYGLTF_BUILD_DIR}/tiny_gltf.cc
)

if(BUILD_AS_LIBRARY)

    add_compile_definitions(EPSILON_BUILD_DLL=1)
    if(MSVC)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    if(CREATE_STATIC_LIB)
        add_library(${PROJECT_NAME} STATIC Epsilon.cpp)
    else()
        add_library(${PROJECT_NAME} SHARED Epsilon.cpp ${PROJECT_FILES})
    endif()

    if(MSVC)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
        target_compile_options(${PROJECT_NAME} PRIVATE /W3)
    else()
        if(WIN32)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
            set(CMAKE_EXE_LINKER_FLAGS "-static -libstdc++")
        endif()
    endif()
else()
    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    endif()
    add_executable(${PROJECT_NAME} main.cpp
        ${PROJECT_FILES} ${PROJECT_ICON})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_precompile_headers(Epsilon 
PRIVATE
    <vector>
    <unordered_map>
    <map>
    <array>
    <set>
    <stack>
    <unordered_set>
    <iostream>
    <fstream>
    <format>
    <string>
    <chrono>
    <cstdint>
    <cstdio>
    <cstring>
    <cstdlib>
    <memory>
    <functional>
    <chrono>
    <mutex>
    <thread>
    <glm/glm.hpp>
    <glm/ext/matrix_transform.hpp>
    <glm/gtc/matrix_transform.hpp>
    <glm/gtc/quaternion.hpp>
    <glm/gtx/quaternion.hpp>
    <glm/common.hpp>
    <glm/ext.hpp>
    <glm/geometric.hpp>
    <glm/gtx/hash.hpp>
    <glm/gtx/compatibility.hpp>
    <random>
    <type_traits>
    <concepts>
    <algorithm>
    <any>
    <core/ui/MaterialIcons_Regular.hpp>
    <vulkan/vulkan_core.h>
    <vulkan/vulkan.hpp>
    <nlohmann/json.hpp>
    <GLFW/glfw3.h>
    <GLFW/glfw3native.h>
    <imgui.h>
    <imgui_internal.h>
    <vk_mem_alloc.h>
)


target_sources(${PROJECT_NAME} PUBLIC
    ${PROJECT_FILES})

find_package(Vulkan REQUIRED)

#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fno-math-errno -g ")

if(NOT BUILD_ANDROID)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})
#endif()

if(BUILD_ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC -landroid)
    target_link_libraries(${PROJECT_NAME} PUBLIC -llog)
endif()

if(DUSE_GLFW)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenAL32.dll)

else()
    target_link_libraries(${PROJECT_NAME} PRIVATE openal)

    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fsized-deallocation")
    #set(CMAKE_CXX_FLAGS_DEBUG "-O2")
endif()


if(DOWNLOAD_LIBRARIES)
    add_dependencies(${PROJECT_NAME} vma)

    if(DUSE_GLFW)
        add_dependencies(${PROJECT_NAME} glfw3_d)
    endif()

    add_dependencies(${PROJECT_NAME} openal_d)
    add_dependencies(${PROJECT_NAME} glm)
    add_dependencies(${PROJECT_NAME} beacon)
    add_dependencies(${PROJECT_NAME} stb)
    #add_dependencies(${PROJECT_NAME} ktx_d)
    add_dependencies(${PROJECT_NAME} json)
    add_dependencies(${PROJECT_NAME} zlib_d)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})

if(NOT BUILD_ANDROID)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

endif()
if(!WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC -rdynamic)
endif()


set(LZLIB_NAME zlib)
set(LZLIBSTATIC_NAME "zlibstatic")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(LZLIB_NAME zlibd)
    set(LZLIBSTATIC_NAME "zlibstaticd")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LZLIBSTATIC_NAME})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LZLIB_NAME})
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC libz.so)
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_PROJECT_NAME "Epsilon Engine")

if(DGENERATE_RPM_PACKAGE)
    set(CPACK_GENERATOR RPM)
    set(CPACK_RPM_COMPONENT_INSTALL ON)
    set(CPACK_RPM_PACKAGE_GROUP "Epsilon")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_MAIN_COMPONENT major)
endif()

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
include(CPack)

#execute_process(
#        COMMAND bash -c ${CMAKE_SOURCE_DIR}/build_shaders.sh
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#        OUTPUT_VARIABLE outVar
#)

file(GLOB shaders "${CMAKE_SOURCE_DIR}/assets/shaders/chess/*.spv")
install(FILES ${shaders} DESTINATION "bin/assets/shaders/chess/")

file(GLOB shaders "${CMAKE_SOURCE_DIR}/assets/shaders/*.spv")
install(FILES ${shaders} DESTINATION "bin/assets/shaders/")

file(GLOB shaders "${CMAKE_SOURCE_DIR}/assets/images/*.*")
install(FILES ${shaders} DESTINATION "bin/assets/images/")


file(GLOB images "${CMAKE_SOURCE_DIR}/assets/images")
install(DIRECTORY ${images} DESTINATION "bin/assets")

if(NOT ${IS_CI_ENV})
    message("Adding sub projects")
    add_subdirectory(examples/bsp)
    add_subdirectory(examples/chess)
    add_subdirectory(editor)
    add_subdirectory(projects/TowerDefense)
    #add_subdirectory(projects/uiapp)
endif()
#if(${Mono_FOUND})
#add_subdirectory(examples/mono-managed)
#endif()
#endif()

#if(D__ANDROID__)
#    add_subdirectory(platform/android/app/src/main/cpp)
#endif()
