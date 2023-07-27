# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "J:/Code/Epsilon-Engine/android-build/json-prefix/src/json"
  "J:/Code/Epsilon-Engine/android-build/json-prefix/src/json-build"
  "J:/Code/Epsilon-Engine/android-build/json-prefix"
  "J:/Code/Epsilon-Engine/android-build/json-prefix/tmp"
  "J:/Code/Epsilon-Engine/android-build/json-prefix/src/json-stamp"
  "J:/Code/Epsilon-Engine/android-build/json-prefix/src"
  "J:/Code/Epsilon-Engine/android-build/json-prefix/src/json-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "J:/Code/Epsilon-Engine/android-build/json-prefix/src/json-stamp/${subDir}")
endforeach()
