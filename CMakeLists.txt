cmake_minimum_required(VERSION 3.22.1)
project(ds VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_library(${PROJECT_NAME}
  src/ds/string.cpp
  src/hash/murmur3.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC src)

option(DS_TEST "DS Test Cases" OFF)
option(DS_THREAD "DS Thread Enabled" OFF)

if (DS_THREAD)
  add_library(ds-thread
    src/ds-thread/mutex.cpp
    src/ds-thread/semaphore.cpp
  )
endif (DS_THREAD)

if (DS_TEST)
  include_directories(src)
  add_subdirectory(external/catch2)
  add_subdirectory(tests)
endif (DS_TEST)

