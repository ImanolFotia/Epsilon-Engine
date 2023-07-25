# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/src/openal_d"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/src/openal_d-build"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/tmp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/src/openal_d-stamp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/src"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/src/openal_d-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/src/openal_d-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/openal_d-prefix/src/openal_d-stamp${cfgdir}") # cfgdir has leading slash
endif()
