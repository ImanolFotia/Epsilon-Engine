# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "J:/Code/Epsilon-Engine/android-build/stb-prefix/src/stb"
  "J:/Code/Epsilon-Engine/android-build/stb-prefix/src/stb-build"
  "J:/Code/Epsilon-Engine/android-build/stb-prefix"
  "J:/Code/Epsilon-Engine/android-build/stb-prefix/tmp"
  "J:/Code/Epsilon-Engine/android-build/stb-prefix/src/stb-stamp"
  "J:/Code/Epsilon-Engine/android-build/stb-prefix/src"
  "J:/Code/Epsilon-Engine/android-build/stb-prefix/src/stb-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "J:/Code/Epsilon-Engine/android-build/stb-prefix/src/stb-stamp/${subDir}")
endforeach()
