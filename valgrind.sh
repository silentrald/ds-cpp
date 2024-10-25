#!/bin/bash

tests=(
  "bptree_map_mem"
  "hash_map_mem"
)

for t in ${!tests[@]}; do
  echo "* === Testing ${tests[$i]} === *"
  valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=1 ./build/tests/${tests[$i]}
done
