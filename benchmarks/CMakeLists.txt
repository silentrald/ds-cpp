
add_executable(bptree_map bptree_map.cpp
  ../tests/main.cpp)
target_compile_definitions(bptree_map PRIVATE DS_TEST)
target_link_libraries(bptree_map PRIVATE
  Catch2::Catch2 Catch2::Catch2WithMain
)

