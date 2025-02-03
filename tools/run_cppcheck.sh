#!/bin/sh


if [ ! -d "$results/cppcheck" ]; then
  mkdir -p results/cppcheck
fi


# --suppress=misra* --suppress=duplicateBreak

cppcheck --error-exitcode=1 --check-level=exhaustive --enable=all --inconclusive \
         --addon=./config/cppcheck/misra.json --addon=misc --std=c++20 \
         --suppress=missingIncludeSystem \
         --max-configs=50 --xml 2> results/cppcheck/cppcheck-errors.xml $*
ret=$?

cppcheck-htmlreport --file=results/cppcheck/cppcheck-errors.xml --title=TestCPPCheck --report-dir=results/cppcheck/cppcheck-reports --source-dir=.

chown -R --reference=tests results/cppcheck/*

echo "$0 done."

#exit $ret
exit 0
