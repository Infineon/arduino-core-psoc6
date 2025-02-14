#!/bin/bash
set -x

# Function to create necessary directories
create_dirs() {
  for dir in results/cppcheck results/cppcheck/build results/clang-tidy results/clang-format; do
    [ ! -d "$dir" ] && mkdir -p "$dir"
  done
}

# Common function to handle error count and exit status
handle_errors() {
  local ERROR_COUNT=$1
  local TOOL_NAME=$2

  if [ "$ERROR_COUNT" -ne 0 ]; then
    echo "$TOOL_NAME completed with $ERROR_COUNT error(s)."
    exit 1
  else
    echo "$TOOL_NAME completed successfully."
  fi
}

# -----------------------------------------------------------------------------------------------------------------------------------------
# Function to run cppcheck
# -----------------------------------------------------------------------------------------------------------------------------------------
run_cppcheck() {
  local ERROR_COUNT=0
  create_dirs

  cppcheck --error-exitcode=1 --check-level=exhaustive --enable=all --inconclusive \
           --addon=./config/cppcheck/misra.json --addon=misc --std=c++20 \
           --suppress=missingIncludeSystem --cppcheck-build-dir=results/cppcheck/build \
           -I extras/arduino-core-api/api \
           -I variants/CY8CKIT-062S2-AI/mtb-bsp \
           -I extras/mtb-libs/core-lib/include \
           --suppress=*:extras/arduino-core-api/api/* \
           --suppress=*:variants/CY8CKIT-062S2-AI/mtb-bsp/* \
           --suppress=*:extras/mtb-libs/core-lib/include/* \
           --suppress=unusedFunction --suppress=misra-c2012-2.5 --suppress=missingInclude \
           --max-configs=5 -j4 --xml 2> results/cppcheck/cppcheck-errors.xml $*

  cppcheck-htmlreport --file=results/cppcheck/cppcheck-errors.xml --title=TestCPPCheck --report-dir=results/cppcheck/cppcheck-reports --source-dir=.
  chown -R --reference=tests results/cppcheck/*
  handle_errors $ERROR_COUNT "Cppcheck"
}

# -----------------------------------------------------------------------------------------------------------------------------------------
# Function to run clang-tidy
# -----------------------------------------------------------------------------------------------------------------------------------------
run_clangtidy() {
  local SRC_DIR=$1
  local HEADER_FILTER="(extras/arduino-core-api/api/.*|variants/CY8CKIT-062S2-AI/mtb-bsp/.*|extras/mtb-libs/core-lib/include/.*|extras/mtb-libs/mtb-hal-cat1/include/.*)"
  local ERROR_COUNT=0
  create_dirs

  find $SRC_DIR -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.hpp' | while read -r FILE; do
    echo "Running clang-tidy on $FILE"
    clang-tidy --config-file="config/clang-tidy/.clang-tidy" \
               -header-filter="$HEADER_FILTER" \
               -export-fixes="results/clang-tidy/$(basename "${FILE%.*}").yaml" \
               --fix-errors \
               --extra-arg=-Wno-error=clang-diagnostic-error \
               $FILE
  done

  handle_errors $ERROR_COUNT "Clang-tidy"
}

# -----------------------------------------------------------------------------------------------------------------------------------------
# Function to run clang-format
# -----------------------------------------------------------------------------------------------------------------------------------------
run_clangformat() {
  local SRC_DIR=$1
  local ERROR_COUNT=0

  find $SRC_DIR -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.hpp' | while read -r FILE; do
    echo "Formatting $FILE"
    clang-format -i -style=file:"./config/clang-format/.clang-format" $FILE
  done

  handle_errors $ERROR_COUNT "Clang-format"
}
# -----------------------------------------------------------------------------------------------------------------------------------------
# Main script logic
# -----------------------------------------------------------------------------------------------------------------------------------------
if [ "$#" -lt 1 ]; then
  echo "Usage: $0 {cppcheck|clang-tidy|clang-format} <source_directory>"
  exit 1
fi

COMMAND=$1
shift

case "$COMMAND" in
  cppcheck)
    [ "$#" -ne 1 ] && { echo "Usage: $0 cppcheck <source_directory>"; exit 1; }
    run_cppcheck "$@"
    ;;
  clang-tidy)
    [ "$#" -ne 1 ] && { echo "Usage: $0 clang-tidy <source_directory>"; exit 1; }
    run_clangtidy "$@"
    ;;
  clang-format)
    [ "$#" -ne 1 ] && { echo "Usage: $0 clang-format <source_directory>"; exit 1; }
    run_clangformat "$@"
    ;;
  *)
    echo "Unknown command: $COMMAND"
    echo "Usage: $0 {cppcheck|clang-tidy|clang-format} <source_directory>"
    exit 1
    ;;
esac
