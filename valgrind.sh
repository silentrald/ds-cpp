#!/bin/bash

tests=(
  "bptree_map_mem"
  "hash_map_mem"
)

for test in ${tests[@]}; do
  echo -e "> === Testing $test ===\n"
  valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=1 ./build/tests/$test
  echo -e "\n> Success $test\n"
done
