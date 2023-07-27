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
set(ANDROID_OPENAL "${CMAKE_SOURCE_DIR}/../../../../../../android-build/openal_d-prefix/src/")


include(ExternalProject)

include_directories(${ANDROID_OPENAL}/openal_d/include)
link_directories(${ANDROID_OPENAL}/openal_d-build/)


link_directories(${CMAKE_SOURCE_DIR}/../../../../../../android-build)

include_directories(${THIRDPARTY_DIR/glm})


#Download and include vma
ExternalProject_Add(vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        )

ExternalProject_Get_property(vma SOURCE_DIR)
set(VMA_BUILD_DIR ${SOURCE_DIR})
message(${VMA_BUILD_DIR})

include_directories(${VMA_BUILD_DIR}/include)

include_directories(${THIRDPARTY_DIR}/stb)
include_directories(${THIRDPARTY_DIR}/beacon)
include_directories(${THIRDPARTY_DIR}/json/include)
include_directories(${THIRDPARTY_DIR}/tinygltf)
include_directories(${THIRDPARTY_DIR}/imgui)
include_directories(${THIRDPARTY_DIR}/glm)



set(PROJECT_FILES
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

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/../../../../../../android-build)

target_link_libraries(
        ${PROJECT_NAME}
        # Specifies the target library.
        vulkan
        epsilon
        game-activity::game-activity_static
        # Links the target library to the log library
        # included in the NDK.
        openal
        log
        android)


add_dependencies(${PROJECT_NAME} vma)
