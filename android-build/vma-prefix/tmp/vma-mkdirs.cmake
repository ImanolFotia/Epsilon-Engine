# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/src/vma"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/src/vma-build"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/tmp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/src/vma-stamp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/src"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/src/vma-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/src/vma-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/vma-prefix/src/vma-stamp${cfgdir}") # cfgdir has leading slash
endif()
