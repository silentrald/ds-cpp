cmake_minimum_required(VERSION 3.22.1)
project(ds)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories(src)

add_library(ds
  # src/prime.cpp

  src/ds/string.cpp

  # src/hash/murmur3.cpp
  # src/hash/number.cpp
)

option(DS_TEST "DS Test Cases" OFF)
option(DS_THREAD "DS Thread Enabled" OFF)

if (DS_THREAD)
  add_library(ds-thread
    src/ds-thread/mutex.cpp
    src/ds-thread/semaphore.cpp
  )
endif (DS_THREAD)

if (DS_TEST)
  add_subdirectory(external/catch2)
  add_subdirectory(tests)
endif (DS_TEST)

