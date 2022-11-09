# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/521ab/esp/esp-idf/components/bootloader/subproject"
  "C:/Users/521ab/Documents/GitHub/Remote_PC_Switcher/firmware/build/bootloader"
  "C:/Users/521ab/Documents/GitHub/Remote_PC_Switcher/firmware/build/bootloader-prefix"
  "C:/Users/521ab/Documents/GitHub/Remote_PC_Switcher/firmware/build/bootloader-prefix/tmp"
  "C:/Users/521ab/Documents/GitHub/Remote_PC_Switcher/firmware/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/521ab/Documents/GitHub/Remote_PC_Switcher/firmware/build/bootloader-prefix/src"
  "C:/Users/521ab/Documents/GitHub/Remote_PC_Switcher/firmware/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/521ab/Documents/GitHub/Remote_PC_Switcher/firmware/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
