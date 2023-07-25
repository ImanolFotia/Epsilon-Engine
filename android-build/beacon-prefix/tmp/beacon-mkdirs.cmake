# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/src/beacon"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/src/beacon-build"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/tmp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/src/beacon-stamp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/src"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/src/beacon-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/src/beacon-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/beacon-prefix/src/beacon-stamp${cfgdir}") # cfgdir has leading slash
endif()
