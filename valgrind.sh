#!/bin/bash

tests=(
  "bptree_map_mem"
  "hash_map_mem"
  "string_mem"
)
exit_code=0

for test in ${tests[@]}; do
  echo -e "> === Testing $test ===\n"
  valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=1 ./build/tests/$test
  if [ $? -eq 0 ]
  then
    echo -e "> Success $test\n"
  else
    echo -e "> Failure $test\n"
    exit_code=1
  fi
done

exit $exit_code
