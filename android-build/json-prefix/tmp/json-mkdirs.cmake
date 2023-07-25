# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/src/json"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/src/json-build"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/tmp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/src/json-stamp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/src"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/src/json-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/src/json-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/json-prefix/src/json-stamp${cfgdir}") # cfgdir has leading slash
endif()
