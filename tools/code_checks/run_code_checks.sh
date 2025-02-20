#!/bin/bash
set -x

# Function to create necessary directories
create_dirs() {
  for dir in _results/cppcheck _results/cppcheck/build _results/clang-tidy _results/clang-format; do
    [ ! -d "$dir" ] && mkdir -p "$dir"
  done
}

# -----------------------------------------------------------------------------------------------------------------------------------------
# Function to run cppcheck
# -----------------------------------------------------------------------------------------------------------------------------------------
run_cppcheck() {
  local SRC_DIR=$1
  create_dirs
  shift

  cppcheck --error-exitcode=1 --check-level=exhaustive --enable=all --inconclusive \
           --addon=./config/cppcheck/misra.json --addon=misc --std=c++20 \
           --suppress=missingIncludeSystem --cppcheck-build-dir=_results/cppcheck/build \
           --suppress=unusedFunction --suppress=misra-c2012-2.5 --suppress=missingInclude \
           $@ \
           --max-configs=30 -j4 --xml 2> _results/cppcheck/cppcheck-errors.xml $SRC_DIR

  cppcheck-htmlreport --file=_results/cppcheck/cppcheck-errors.xml --title=TestCPPCheck --report-dir=_results/cppcheck/cppcheck-reports --source-dir=.
  chown -R --reference=tests _results/cppcheck/*
  python3 tools/code_checks/analyze_cppcheck.py -v
}

# -----------------------------------------------------------------------------------------------------------------------------------------
# Function to run clang-tidy
# -----------------------------------------------------------------------------------------------------------------------------------------
run_clangtidy() {
  local SRC_DIR=$1
  shift

  create_dirs

  find $SRC_DIR -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.hpp' | while read -r FILE; do
    echo "Running clang-tidy on $FILE"
    clang-tidy --config-file="config/clang-tidy/.clang-tidy" \
               -export-fixes="_results/clang-tidy/$(basename "${FILE%.*}").yaml" \
               $@ \
               --extra-arg=-Wno-error=clang-diagnostic-error \
               -p . \
               $FILE
  done

}

# -----------------------------------------------------------------------------------------------------------------------------------------
# Function to run clang-format
# -----------------------------------------------------------------------------------------------------------------------------------------
run_clangformat() {
  local SRC_DIR=$1

  find $SRC_DIR -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.hpp' | while read -r FILE; do
    echo "Formatting $FILE"
    clang-format -i -style=file:"./config/clang-format/.clang-format" $FILE
  done

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
    [ "$#" -lt 1 ] && { echo "Usage: $0 cppcheck <source_directory> <>"; exit 1; }
    run_cppcheck "$@"
    ;;
  clang-tidy)
    [ "$#" -lt 1 ] && { echo "Usage: $0 clang-tidy <source_directory> <>"; exit 1; }
    run_clangtidy "$@"
    ;;
  clang-format)
    [ "$#" -lt 1 ] && { echo "Usage: $0 clang-format <source_directory> <>"; exit 1; }
    run_clangformat "$@"
    ;;
  black-format)
    [ "$#" -lt 1 ] && { echo "Usage: $0 clang-format <source_directory> <>"; exit 1; }
    echo "Formatting python scripts"
    black $@
    git diff --exit-code
    ;;
  *)
    echo "Unknown command: $COMMAND"
    echo "Usage: $0 {cppcheck|clang-tidy|clang-format} <source_directory>"
    exit 1
    ;;
esac
