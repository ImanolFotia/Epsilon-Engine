<<<<<<< HEAD
# Install script for directory: H:/Epsilon/Epsilon Engine/thirdparty/openal-soft
=======
# Install script for directory: C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/OpenAL")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
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

<<<<<<< HEAD
# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "H:/msys2/msys64/msys64/mingw64/bin/objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/libOpenAL32.dll.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/OpenAL32.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/OpenAL32.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/OpenAL32.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "H:/msys2/msys64/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/OpenAL32.dll")
=======
if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/libOpenAL32.dll.a")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/OpenAL32.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/OpenAL32.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/OpenAL32.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/OpenAL32.dll")
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed
    endif()
  endif()
endif()

<<<<<<< HEAD
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL/OpenALConfig.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL/OpenALConfig.cmake"
         "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/CMakeFiles/Export/lib/cmake/OpenAL/OpenALConfig.cmake")
=======
if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL/OpenALConfig.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL/OpenALConfig.cmake"
         "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/CMakeFiles/Export/lib/cmake/OpenAL/OpenALConfig.cmake")
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL/OpenALConfig-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL/OpenALConfig.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
<<<<<<< HEAD
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL" TYPE FILE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/CMakeFiles/Export/lib/cmake/OpenAL/OpenALConfig.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL" TYPE FILE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/CMakeFiles/Export/lib/cmake/OpenAL/OpenALConfig-relwithdebinfo.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/AL" TYPE FILE FILES
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/include/AL/al.h"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/include/AL/alc.h"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/include/AL/alext.h"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/include/AL/efx.h"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/include/AL/efx-creative.h"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/include/AL/efx-presets.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/openal.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openal" TYPE FILE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/alsoftrc.sample")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openal/hrtf" TYPE FILE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/hrtf/Default HRTF.mhr")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openal/presets" TYPE FILE FILES
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/presets/3D7.1.ambdec"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/presets/hexagon.ambdec"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/presets/itu5.1.ambdec"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/presets/itu5.1-nocenter.ambdec"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/presets/rectangle.ambdec"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/presets/square.ambdec"
    "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/presets/presets.txt"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/openal-info.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openal-info.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openal-info.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "H:/msys2/msys64/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openal-info.exe")
=======
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL" TYPE FILE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/CMakeFiles/Export/lib/cmake/OpenAL/OpenALConfig.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenAL" TYPE FILE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/CMakeFiles/Export/lib/cmake/OpenAL/OpenALConfig-relwithdebinfo.cmake")
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/AL" TYPE FILE FILES
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/include/AL/al.h"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/include/AL/alc.h"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/include/AL/alext.h"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/include/AL/efx.h"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/include/AL/efx-creative.h"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/include/AL/efx-presets.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/openal.pc")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openal" TYPE FILE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/alsoftrc.sample")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openal/hrtf" TYPE FILE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/hrtf/Default HRTF.mhr")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openal/presets" TYPE FILE FILES
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/presets/3D7.1.ambdec"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/presets/hexagon.ambdec"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/presets/itu5.1.ambdec"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/presets/itu5.1-nocenter.ambdec"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/presets/rectangle.ambdec"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/presets/square.ambdec"
    "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/presets/presets.txt"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/openal-info.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openal-info.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openal-info.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openal-info.exe")
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed
    endif()
  endif()
endif()

<<<<<<< HEAD
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/altonegen.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/altonegen.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/altonegen.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "H:/msys2/msys64/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/altonegen.exe")
=======
if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/altonegen.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/altonegen.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/altonegen.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/altonegen.exe")
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed
    endif()
  endif()
endif()

<<<<<<< HEAD
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/alrecord.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/alrecord.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/alrecord.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "H:/msys2/msys64/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/alrecord.exe")
=======
if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/alrecord.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/alrecord.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/alrecord.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/alrecord.exe")
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
<<<<<<< HEAD
  include("H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/utils/alsoft-config/cmake_install.cmake")
=======
  include("C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/utils/alsoft-config/cmake_install.cmake")
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
<<<<<<< HEAD
file(WRITE "H:/Epsilon/Epsilon Engine/thirdparty/openal-soft/build/${CMAKE_INSTALL_MANIFEST}"
=======
file(WRITE "C:/Users/Imanol/Documents/Code/C++/Epsilon/deps-64/openal-soft/build/${CMAKE_INSTALL_MANIFEST}"
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
