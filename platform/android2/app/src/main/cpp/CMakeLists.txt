# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 20)

# Declares and names the project.

project("epsilon-android")


find_package(game-activity REQUIRED CONFIG)
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")


include_directories(${CMAKE_SOURCE_DIR}/../../../../../..)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/../../../../../../thirdparty")


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

include_directories(${VMA_BUILD_DIR}/include)


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


#Download and include Beacon
ExternalProject_Add(beacon
        GIT_REPOSITORY https://github.com/ImanolFotia/beacon.git
        INSTALL_COMMAND cmake -E echo "Skipping install step for beacon."
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        )

ExternalProject_Get_property(beacon SOURCE_DIR)
set(BEACON_BUILD_DIR ${SOURCE_DIR})
message(${BEACON_BUILD_DIR})

include_directories(${BEACON_BUILD_DIR})


#Download and include nlohmann/json
ExternalProject_Add(json
        GIT_REPOSITORY https://www.github.com/nlohmann/json.git
        INSTALL_COMMAND cmake -E echo "Skipping install step for json."
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        )

ExternalProject_Get_property(json SOURCE_DIR)
set(JSON_BUILD_DIR ${SOURCE_DIR})
message(${JSON_BUILD_DIR})

include_directories(${JSON_BUILD_DIR}/include)


include(ExternalProject)


set(IMGUI_BUILD_DIR ${THIRDPARTY_DIR}/imgui)
message(${IMGUI_BUILD_DIR})

include_directories(${IMGUI_BUILD_DIR})
link_directories(${IMGUI_BUILD_DIR})


set(TINYGLTF_BUILD_DIR ${THIRDPARTY_DIR}/tinygltf)
message(${TINYGLTF_BUILD_DIR})
include_directories(${TINYGLTF_BUILD_DIR})


set(CMAKE_OPENAL_ARGS "")
if(WIN32)
    set(CMAKE_OPENAL_ARGS "")
else()
    set(CMAKE_OPENAL_ARGS "-DALSOFT_REQUIRE_PULSEAUDIO=ON -DALSOFT_REQUIRE_PIPEWIRE=ON")
endif()

# download and include openal
#requires installation of pulse audio and pipewire developer libs in the OS
ExternalProject_Add(openal_d
        GIT_REPOSITORY https://github.com/kcat/openal-soft.git
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


set(PROJECT_FILES
        ${CMAKE_SOURCE_DIR}/core/engine/context.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/env.cpp
        ${CMAKE_SOURCE_DIR}/main_lib.cpp
        ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/vulkan_impl.cpp
        ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/vulkan.cpp
        ${CMAKE_SOURCE_DIR}/core/engine/renderer/vulkan/resource_manager.cpp
        ${CMAKE_SOURCE_DIR}/core/engine/audio/al/audio_manager.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/IO.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyBoard.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyBoard.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/KeyboardWrapper.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Mouse.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/Joystick.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/JoystickWrapper.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/Joystick/Mappings.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/IO/IO.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/utils/helpers/camera.cpp
        ${CMAKE_SOURCE_DIR}/core/framework/utils/helpers/camera.hpp
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
        ${CMAKE_SOURCE_DIR}/modules/mono/mono-instance.hpp
        ${CMAKE_SOURCE_DIR}/Epsilon.cpp
        ${CMAKE_SOURCE_DIR}/Epsilon.hpp
        ${CMAKE_SOURCE_DIR}/core/framework/singleton.hpp
        ${CMAKE_SOURCE_DIR}/modules/imgui/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/modules/imgui/imgui_impl_vulkan.cpp
        ${IMGUI_BUILD_DIR}/ImPlot/implot_items.cpp
        ${IMGUI_BUILD_DIR}/ImPlot/implot.cpp
        ${IMGUI_BUILD_DIR}/imgui.cpp
        ${IMGUI_BUILD_DIR}/imgui_demo.cpp
        ${IMGUI_BUILD_DIR}/imgui_draw.cpp
        ${IMGUI_BUILD_DIR}/imgui_tables.cpp
        ${IMGUI_BUILD_DIR}/imgui_widgets.cpp
        ${TINYGLTF_BUILD_DIR}/tiny_gltf.cc
        )


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=1)


find_library( # Sets the name of the path variable.
        log-lib
        vulkan
        android
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


add_library( # Sets the name of the library.
        ${PROJECT_NAME}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        AndroidApp.cpp)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
#add_subdirectory(../../../../../../ ../../../../../../build EXCLUDE_FROM_ALL)

target_link_libraries(
        ${PROJECT_NAME}
        # Specifies the target library.
        vulkan
        epsilon
        game-activity::game-activity_static
        # Links the target library to the log library
        # included in the NDK.
        log
        android)


add_dependencies(${PROJECT_NAME} vma)

add_dependencies(${PROJECT_NAME} openal_d)
if(D__ANDROID__)
    add_dependencies(${PROJECT_NAME} openal_d)
    add_dependencies(${PROJECT_NAME} assimp_d)
else()

endif()
add_dependencies(${PROJECT_NAME} glm)
add_dependencies(${PROJECT_NAME} beacon)
add_dependencies(${PROJECT_NAME} stb)
add_dependencies(${PROJECT_NAME} json)