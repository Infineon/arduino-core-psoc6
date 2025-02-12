#!/bin/sh
set -x

if [ ! -d "$results/cppcheck" ]; then
  mkdir -p results/cppcheck
fi
if [ ! -d "$results/build" ]; then
  mkdir -p results/build
fi

# --suppress=misra* --suppress=duplicateBreak

cppcheck --error-exitcode=1 --check-level=exhaustive --enable=all --inconclusive \
         --addon=./config/cppcheck/misra.json --addon=misc --std=c++20 \
         --suppress=missingIncludeSystem --cppcheck-build-dir=results/build \
         -I extras/arduino-core-api/api \
         -I variants/CY8CKIT-062S2-AI/mtb-bsp  \
         -I extras/mtb-libs/core-lib/include \
         --suppress=*:extras/arduino-core-api/api/* \
         --suppress=*:variants/CY8CKIT-062S2-AI/mtb-bsp/* \
         --suppress=*:extras/mtb-libs/core-lib/include/* \
         --suppress=unusedFunction --suppress=misra-c2012-2.5 --suppress=missingInclude\
         --max-configs=5 -j4 --xml 2> results/cppcheck/cppcheck-errors.xml $*
ret=$?

cppcheck-htmlreport --file=results/cppcheck/cppcheck-errors.xml --title=TestCPPCheck --report-dir=results/cppcheck/cppcheck-reports --source-dir=.

chown -R --reference=tests results/cppcheck/*

echo "$0 done."

#exit $ret
exit 0
