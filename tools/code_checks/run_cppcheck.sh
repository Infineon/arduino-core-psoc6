#!/bin/bash


echo "executing $0 ..."


usage() {
  echo "Usage: $0 :" 1>&2;
  echo "          [-a | --addon] <addon>  [--check-level] <check level>  [-e | --enable] <checks to enable>  [--error-exitcode] <Error exitcode>  [ --exitcode-suppressions] <File for messages not to generate error exit code.>  [--file-filter] <File filter pattern.>  [--file-list] <File list to process.>  [--file-prefix] <file_prefix>" 1>&2;
  echo "          [-I] <Directory to search for include files.>  [-i] <Source files or directory not to be processed.>  [--includes-file] <Search paths to header files.>  [--inconclusive] <inconclusive flag>  [--max-configs] < max configs>  [--no-addon-default]  [--no-enable-default]  [--no-suppress-default]  [-o | --output-dir] <output directory>" 1>&2;
  echo "          [--std] <c++ standard>  [-s | --suppress] <suppress messages>  [--suppressions-list] <File list.>  [--use-shell-color] <Use color in terminal.>" 1>&2;
  exit 1;
}


# This requires GNU getopt.
TEMP=`getopt -o I:a:e:i:o:s: --long addon:,check-level:,enable:,error-exitcode:,excludes:,exitcode-suppressions:,file-filter:,file-list:,file-prefix:,includes-file:,inconclusive,max-configs:,no-addon-default,no-enable-default,no-suppress-default,output-dir:,suppress:,--suppressions-list:,std:,use-shell-color -n "$0" -- "$@"`

if [ $? != 0 ] ; then
  echo "Terminating..." >&2 ;
  usage ;
  exit 1 ; 
fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"


addon=
addon_default="--addon=config/cppcheck/misra.json --addon=misc"
check_level="--check-level=exhaustive"
enable=
enable_default="--enable=information,missingInclude,performance,portability,style,warning"
error_exitcode="--error-exitcode=2"
excludes=
exitcode_suppressions=
file_filter=
file_list=
file_prefix=
includes=
includes_file=
inconclusive="--inconclusive"
max_configs="--max-configs=50"
output_dir="_results/cppcheck"
std="--std=c++20"
suppress=
suppress_default="--suppress=misra-c2012-2.5 --suppress=missingIncludeSystem"
suppressions_list=
unbuffer=
use_shell_color=false


while true; do
  case "$1" in
    -a | --addon )                  addon+=" --addon=$2";                                shift 2 ;;
         --check-level )            check_level="--check-level=$2";                      shift 2 ;;
    -e | --enable )                 enable+=" --enable=$2";                              shift 2 ;;
         --error-exitcode )         error_exitcode="--error-exitcode=$2";                shift 2 ;;
         --exitcode-suppressions )  exitcode_suppressions="--exitcode-suppressions=$2";  shift 2 ;;
         --file-filter )            file_filter+=" --file-filter=$2";                    shift 2 ;;
         --file-list )              file_list="--file-list=$2";                          shift 2 ;;
         --file-prefix )            file_prefix=$2;                                      shift 2 ;;
    -I )                            includes+=" -I $2";                                  shift 2 ;;
    -i )                            excludes+=" -i $2";                                  shift 2 ;;
         --includes-file )          includes_file+=" --includes-file=$2";                shift 2 ;;
         --inconclusive )           inconclusive="--inconclusive";                       shift ;;
         --max-configs )            max_configs="--max-configs=$2";                      shift 2 ;;
         --no-addon-default )       addon_default="";                                    shift ;;
         --no-enable-default )      enable_default="";                                   shift ;;
         --no-suppress-default )    suppress_default="";                                 shift ;;
    -o | --output-dir )             output_dir=$2;                                       shift 2 ;;
         --std )                    std="--std=$2";                                      shift 2 ;;
    -s | --suppress )               suppress+=" --suppress=$2";                          shift 2 ;;
         --suppressions-list )      suppressions_list="--suppressions-list=$2";          shift 2 ;;
         --use-shell-color )        use_shell_color=true; unbuffer="unbuffer";           shift ;;
         -- )                                                                            shift;      break ;;
    * )                             echo "Unknown option '$1' found !" ; usage ;                     break ;;
  esac
done


addon="$addon_default $addon"
enable="$enable_default $enable"
suppress="$suppress_default $suppress"


if [ -z "$output_dir" ]; then
  usage
fi

if [ ! -z "$file_prefix" ]; then
  output_dir+="/$file_prefix"
else
  file_prefix="cppcheck"
fi

if [ ! -d "$output_dir" ]; then
  mkdir -p $output_dir/html-report
  mkdir -p $output_dir/build
fi


echo "addon                 : $addon"
echo "check-level           : $check_level"
echo "enable                : $enable"
echo "error-exitcode        : $error_exitcode"
echo "excludes              : $excludes"
echo "exitcode-suppressions : $exitcode_suppressions"
echo "file-filter           : $file_filter"
echo "file-list             : $file_list"
echo "file-prefix           : $file_prefix"
echo "includes              : $includes"
echo "includes-file         : $includes_file"
echo "inconclusive          : $inconclusive"
echo "max-configs           : $max_configs"
echo "output-dir            : $output_dir"
echo "std                   : $std"
echo "suppress              : $suppress"
echo "suppressions-list     : $suppressions_list"
echo "use-shell-color       : $use_shell_color"
echo ""


returnValue=0


$unbuffer cppcheck $addon $check_level $enable $error_exitcode $excludes $exitcode_suppressions $file_filter $file_list $includes $inconclusive \
                  $max_configs $std $suppress $suppressions_list --cppcheck-build-dir=$output_dir/build -j4 --xml --output-file=$output_dir/$file_prefix-errors.xml $* 2>&1 | tee $output_dir/$file_prefix.log
returnValue=${PIPESTATUS[0]}

echo "" | tee -a $output_dir/$file_prefix.log

python3 ./tools/code_checks/merge_clang_tidy_cppcheck.py --logDir=$output_dir/../../clang-tidy/check-clang-tidy/ --xmlPath=$output_dir/$file_prefix-errors.xml 
cppcheck-htmlreport --file=$output_dir/$file_prefix-errors.xml --title=CPPCheck --report-dir=$output_dir/html-report --source-dir=. 2>&1 | tee -a $output_dir/$file_prefix.log

chown -R --reference=`dirname $*` $output_dir

echo "$0 done."
exit $returnValue
