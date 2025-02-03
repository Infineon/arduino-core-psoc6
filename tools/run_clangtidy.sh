#!/bin/sh
set -x

SRC_FILES=$1

# Initialize an error counter
ERROR_COUNT=0

HEADER_FILTER="./cores/psoc/"
BASE_INCLUDE_PATHS="-extra-arg -I./extras/arduino-core-api/api -extra-arg -I./extras/mtb-libs/mtb-hal-cat1/COMPONENT_CAT1A/include/pin_packages -extra-arg -I./extras/mtb-libs/mtb-hal-cat1/COMPONENT_CAT1A/include"

# Iterate over each file and run clang-tidy
for FILE in $SRC_FILES; do
  echo "Running clang-tidy on $FILE"
  FILENAME=$(basename "${FILE%.*}")
  clang-tidy --config-file="config/clang-tidy/.clang-tidy" -header-filter="$HEADER_FILTER" -export-fixes="results/clang-tidy/$FILENAME.yaml" $FILE $BASE_INCLUDE_PATHS -- 
  if [ $? -ne 0 ]; then
    echo "ERROR: clang-tidy failed for $FILE"
    ERROR_COUNT=$((ERROR_COUNT + 1))
  fi
done

# Check if there were any errors and exit with a non-zero status if there were
if [ $ERROR_COUNT -ne 0 ]; then
  echo "Clang-tidy completed with $ERROR_COUNT error(s)."
  exit 1
else
  echo "Clang-tidy completed successfully on all files."
fi
