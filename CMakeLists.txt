cmake_minimum_required(VERSION 3.22.1)
project(ds)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories(src)

add_library(ds
  src/prime.cpp

  src/ds/string.cpp
  src/ds/scope.cpp
  src/ds-error/error.cpp

  src/hash/murmur3.cpp
  src/hash/number.cpp
)

option(DS_TEST "DS Test Cases" OFF)
option(DS_GEOM "DS Geom Enabled" OFF)
option(DS_THREAD "DS Thread Enabled" OFF)

if (NOT DS_ERROR_MSG)
  set(DS_ERROR_MSG 256)
endif (NOT DS_ERROR_MSG)
target_compile_definitions(${PROJECT_NAME} PRIVATE DS_ERROR_MSG=${DS_ERROR_MSG})

if (DS_TEST)
  add_subdirectory(external/catch2)
  add_subdirectory(tests)
endif(DS_TEST)

