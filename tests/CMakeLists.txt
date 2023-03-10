
# Bptree Map
add_executable(bptree_map bptree_map.cpp)
# target_link_libraries(fixed_deque PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

# Fixed Deque
add_executable(fixed_deque fixed_deque.cpp main.cpp)
target_link_libraries(fixed_deque PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

# Hash Map
add_executable(hash_map hash_map.cpp main.cpp
  ../src/hash/number.cpp
  ../src/hash/murmur3.cpp
  
  ../src/ds/string.cpp

  ../src/ds/prime.cpp
)
target_link_libraries(hash_map PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

# Hash Set
add_executable(hash_set hash_set.cpp main.cpp
  ../src/hash/number.cpp
  ../src/hash/murmur3.cpp
  
  ../src/ds/string.cpp

  ../src/ds/prime.cpp
)
target_link_libraries(hash_set PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

add_executable(shared_ptr shared_ptr.cpp main.cpp)
target_link_libraries(shared_ptr PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

# String
add_executable(string string.cpp main.cpp
  ../src/ds/string.cpp
)
target_compile_definitions(string PRIVATE DS_TEST)
target_link_libraries(string PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

# Vector
add_executable(vector vector.cpp main.cpp
  ../src/ds/string.cpp
)
target_compile_definitions(vector PRIVATE DS_TEST)
target_link_libraries(vector PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

