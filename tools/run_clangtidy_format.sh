#!/bin/sh
set -x

SRC_FILES=$1
HEADER_FILTER="(extras/arduino-core-api/api/.*|variants/CY8CKIT-062S2-AI/mtb-bsp/.*|extras/mtb-libs/core-lib/include/.*|extras/mtb-libs/mtb-hal-cat1/include/.*)"
ERROR_COUNT=0

# Iterate over each file and run clang-format
for FILE in $SRC_FILES; do
  echo "Running clang-format formatting on $FILE"
  FILENAME=$(basename "${FILE%.*}")
  clang-format "config/clang-format/.clang-format" -i \
             $FILE 
  if [ $? -ne 0 ]; then
    echo "ERROR: clang-format failed for $FILE"
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
