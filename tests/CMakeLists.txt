add_compile_definitions(DS_ERROR_MSG=${DS_ERROR_MSG})

# Bptree Map
# add_executable(bptree_map bptree_map.cpp
#   ../src/ds-error/error.cpp)
# target_link_libraries(bptree_map PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

# Fixed Deque
# add_executable(fixed_deque fixed_deque.cpp main.cpp
#   ../src/ds-error/error.cpp)
# target_link_libraries(fixed_deque PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

# Hash Map
# add_executable(hash_map hash_map.cpp main.cpp
#   ../src/hash/number.cpp
#   ../src/hash/murmur3.cpp
#   
#   ../src/ds/string.cpp
#   ../src/ds-error/error.cpp
#
#   ../src/prime.cpp
# )
# target_link_libraries(hash_map PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

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

# add_executable(shared_ptr shared_ptr.cpp main.cpp
#   ../src/ds-error/error.cpp)
# target_link_libraries(shared_ptr PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

# String
add_executable(string string.cpp main.cpp
  ../src/ds/string.cpp
)
target_compile_definitions(string PRIVATE DS_TEST)
target_link_libraries(string PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

# Unique_ptr
# add_executable(unique_ptr unique_ptr.cpp main.cpp
#   ../src/ds-error/error.cpp)
# target_link_libraries(unique_ptr PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

# Vector
# add_executable(vector vector.cpp main.cpp
#   ../src/ds/string.cpp
#   ../src/ds-error/error.cpp
# )
# target_compile_definitions(vector PRIVATE DS_TEST)
# target_link_libraries(vector PRIVATE
#   Catch2::Catch2 Catch2::Catch2WithMain
# )

# Geometry
# add_executable(geom geom.cpp
#   ../src/ds-error/error.cpp)

