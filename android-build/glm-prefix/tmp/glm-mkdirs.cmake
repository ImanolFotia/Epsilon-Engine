# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/src/glm"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/src/glm-build"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/tmp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/src/glm-stamp"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/src"
  "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/src/glm-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/src/glm-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/solaire/Documents/Code/C++/Epsilon-Engine/android-build/glm-prefix/src/glm-stamp${cfgdir}") # cfgdir has leading slash
endif()
