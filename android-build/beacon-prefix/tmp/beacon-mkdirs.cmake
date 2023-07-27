# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "J:/Code/Epsilon-Engine/android-build/beacon-prefix/src/beacon"
  "J:/Code/Epsilon-Engine/android-build/beacon-prefix/src/beacon-build"
  "J:/Code/Epsilon-Engine/android-build/beacon-prefix"
  "J:/Code/Epsilon-Engine/android-build/beacon-prefix/tmp"
  "J:/Code/Epsilon-Engine/android-build/beacon-prefix/src/beacon-stamp"
  "J:/Code/Epsilon-Engine/android-build/beacon-prefix/src"
  "J:/Code/Epsilon-Engine/android-build/beacon-prefix/src/beacon-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "J:/Code/Epsilon-Engine/android-build/beacon-prefix/src/beacon-stamp/${subDir}")
endforeach()
