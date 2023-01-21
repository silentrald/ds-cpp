cmake_minimum_required(VERSION 3.22.1)
project(ds)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Unit Testing
add_subdirectory(external/catch2)
include_directories(src)

set(DS_TEST "DS Test Cases" ON)

if (DS_TEST)
  # Bptree Map
  add_executable(bptree_map src/bptree_map/test.cpp)
  set_target_properties(bptree_map PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  # target_link_libraries(fixed_deque PRIVATE
  #   Catch2::Catch2
  #   Catch2::Catch2WithMain
  # )

  # Fixed Deque
  add_executable(fixed_deque src/fixed_deque/test.cpp src/test.cpp)
  set_target_properties(fixed_deque PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  target_link_libraries(fixed_deque PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
  )

  # Hash Map
  add_executable(hash_map src/hash_map/test.cpp src/test.cpp
    src/hash/number.cpp
    src/hash/murmur3.cpp
    
    src/string/impl.cpp

    src/prime.cpp
  )
  set_target_properties(hash_map PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  target_link_libraries(hash_map PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
  )

  # Hash Set
  add_executable(hash_set src/hash_set/test.cpp src/test.cpp
    src/hash/number.cpp
    src/hash/murmur3.cpp
    
    src/string/impl.cpp

    src/prime.cpp
  )
  set_target_properties(hash_set PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  target_link_libraries(hash_set PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
  )

  add_executable(shared_ptr src/shared_ptr/test.cpp src/test.cpp)
  set_target_properties(shared_ptr PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  target_link_libraries(shared_ptr PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
  )

  # String
  add_executable(string src/string/test.cpp src/test.cpp
    src/string/impl.cpp
  )
  target_compile_definitions(string PRIVATE DS_TEST)
  set_target_properties(string PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  target_link_libraries(string PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
  )

  # Vector
  add_executable(vector src/vector/test.cpp src/test.cpp
    src/string/impl.cpp
  )
  target_compile_definitions(vector PRIVATE DS_TEST)
  set_target_properties(vector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  target_link_libraries(vector PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
  )
endif(DS_TEST)

