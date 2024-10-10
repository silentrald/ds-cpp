# Bptree Map
add_executable(tests main.cpp
  # String
  ../src/ds/string.cpp

  # Smart pointers
  shared_ptr.cpp
  unique_ptr.cpp

  # Hash
  ../src/hash/murmur3.cpp

  # DS Container Tests
  bptree_map.cpp
  hash_map.cpp
  string.cpp
  vector.cpp
)
target_compile_definitions(tests PRIVATE DS_TEST)
target_link_libraries(tests PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

add_executable(bptree_map_mem bptree_map_mem.cpp main.cpp)
target_compile_definitions(bptree_map_mem PRIVATE DS_TEST)
target_link_libraries(bptree_map_mem PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

add_executable(hash_map_mem hash_map_mem.cpp main.cpp
  ../src/hash/murmur3.cpp
  ../src/ds/string.cpp
)
target_compile_definitions(hash_map_mem PRIVATE DS_TEST)
target_link_libraries(hash_map_mem PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

# Hash Set
# add_executable(hash_set hash_set.cpp main.cpp
#   ../src/hash/number.cpp
#   ../src/hash/murmur3.cpp
#   
#   ../src/ds/string.cpp
#   ../src/ds-error/error.cpp
#
#   ../src/prime.cpp
# )
# target_link_libraries(hash_set PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

# Geometry
# add_executable(geom geom.cpp
#   ../src/ds-error/error.cpp)

