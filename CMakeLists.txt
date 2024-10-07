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
option(DS_BENCHMARK "DS Benchmarks" OFF)
option(DS_GEOM "DS Geom Enabled" OFF)
option(DS_THREAD "DS Thread Enabled" OFF)

if (DS_THREAD)
  add_library(ds-thread
    src/ds-thread/mutex.cpp
    src/ds-thread/semaphore.cpp
  )
endif (DS_THREAD)

if (DS_TEST OR DS_BENCHMARK)
  add_subdirectory(external/catch2)
endif (DS_TEST OR DS_BENCHMARK)

if (DS_TEST)
  add_subdirectory(tests)
endif (DS_TEST)

if (DS_BENCHMARK)
  set(DS_TEST ON)
  add_subdirectory(benchmarks)
  set(DS_TEST OFF)
endif (DS_BENCHMARK)
