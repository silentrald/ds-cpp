cmake_minimum_required(VERSION 3.22.1)
project(ds)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories(src)

add_library(ds
  src/ds/string.cpp
  src/ds/prime.cpp
  src/ds/transaction.cpp

  src/hash/murmur3.cpp
  src/hash/number.cpp
)

option(DS_TEST "DS Test Cases" OFF)

if (DS_TEST)
  add_subdirectory(external/catch2)
  add_subdirectory(tests)
endif(DS_TEST)

