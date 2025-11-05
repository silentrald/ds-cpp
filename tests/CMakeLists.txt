# Bptree Map
add_executable(tests main.cpp test.cpp
  # String
  ../src/ds/string.cpp

  # Smart pointers
  shared_ptr.cpp
  unique_ptr.cpp

  # Hash
  ../src/hash/murmur3.cpp

  # DS Container Tests
  # bptree_map.cpp
  hash_map.cpp
  hash_set.cpp
  string.cpp
  vector.cpp

  # Sorting Tests
  sort.cpp

  # Benchmarks
  # ../benchmarks/bptree_map.cpp
)
target_compile_definitions(tests PRIVATE DS_TEST)
target_link_libraries(tests PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

# Valgrind Checks
# add_executable(bptree_map_mem bptree_map_mem.cpp)
# target_compile_definitions(bptree_map_mem PRIVATE DS_TEST)

add_executable(hash_map_mem hash_map_mem.cpp test.cpp
  ../src/hash/murmur3.cpp
  ../src/ds/string.cpp
)
target_compile_definitions(hash_map_mem PRIVATE DS_TEST)

add_executable(string_mem string_mem.cpp test.cpp
  ../src/ds/string.cpp
)
target_compile_definitions(string_mem PRIVATE DS_TEST)
