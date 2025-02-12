#!/bin/sh
set -x

SRC_DIR=$1
ERROR_COUNT=0

# Iterate over each file and run clang-format
find $SRC_DIR -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.hpp' | while read FILE; do
  echo "Formatting $FILE"
  clang-format -i -style=file:"./config/clang-format/.clang-format" $FILE 
  
  if [ $? -ne 0 ]; then
    echo "ERROR: clang-format failed for $FILE"
    ERROR_COUNT=$((ERROR_COUNT + 1))
  fi
done

# Check if there were any errors and exit with a non-zero status if there were
if [ $ERROR_COUNT -ne 0 ]; then
  echo "Clang-format failed with $ERROR_COUNT error(s)."
  exit 1
else
  echo "All files formatted."
fi
