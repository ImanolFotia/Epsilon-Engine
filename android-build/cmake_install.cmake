# Install script for directory: J:/Code/Epsilon-Engine

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "J:/Code/Epsilon-Engine/android-build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "J:/AndroidSDK/ndk/26.0.10404224-beta1/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libEpsilon.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libEpsilon.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libEpsilon.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "J:/Code/Epsilon-Engine/android-build/libEpsilon.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libEpsilon.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libEpsilon.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "J:/AndroidSDK/ndk/26.0.10404224-beta1/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libEpsilon.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/assets/shaders/chess" TYPE FILE FILES
    "J:/Code/Epsilon-Engine/assets/shaders/chess/board-fragment.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/chess/board-vertex.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/chess/piece-fragment.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/chess/piece-vertex.spv"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/assets/shaders" TYPE FILE FILES
    "J:/Code/Epsilon-Engine/assets/shaders/board-fragment.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/board-vertex.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/fragment.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/shadow-fragment.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/shadow-vertex.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/sky-fragment.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/sky-vertex.spv"
    "J:/Code/Epsilon-Engine/assets/shaders/vertex.spv"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/assets/images" TYPE FILE FILES
    "J:/Code/Epsilon-Engine/assets/images/diff.png"
    "J:/Code/Epsilon-Engine/assets/images/maxresdefault.png"
    "J:/Code/Epsilon-Engine/assets/images/pieces.png"
    "J:/Code/Epsilon-Engine/assets/images/texture.png"
    "J:/Code/Epsilon-Engine/assets/images/texture2.png"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/assets" TYPE DIRECTORY FILES "J:/Code/Epsilon-Engine/assets/images")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("J:/Code/Epsilon-Engine/android-build/examples/bsp/cmake_install.cmake")
  include("J:/Code/Epsilon-Engine/android-build/projects/TowerDefense/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "J:/Code/Epsilon-Engine/android-build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
