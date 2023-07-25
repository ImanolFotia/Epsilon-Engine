# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/src/stb"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/src/stb-build"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/tmp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/src/stb-stamp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/src"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/src/stb-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/src/stb-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/stb-prefix/src/stb-stamp${cfgdir}") # cfgdir has leading slash
endif()
