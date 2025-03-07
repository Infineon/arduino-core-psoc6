#!/bin/bash


echo "executing $* ..."


usage() {
  echo "Usage: $0 :" 1>&2;
  echo "          [--checks] <Checks.>  [--config-file] <Config-file.>  [--export-fixes] <Export fixes.>  [--extra-arg] <Extra argument.>  [--file-prefix] <File_prefix>  [--fix] <Fix reported issues.>" 1>&2;
  echo "          [--header-filter] <Header filter regex for headers to be processed.>  [-I] <Directory to search for include files.>  [-i] <Source files or directory not to be processed.>" 1>&2;
  echo "          [-o | --output-dir] <output directory>  [--quiet] <Quiet.>  [--system-headers] <Process system headers.>  [--use-color] <Use color.>  [--use-shell-color] <Use color in terminal.>" 1>&2;
  echo "          [--warnings-as-errors]  <Upgrade warnings to errors for specified checks.> " 1>&2;
  exit 1;
}


# This requires GNU getopt.
TEMP=`getopt -o I:i:o: --long checks:,config-file:,export-fixes:,extra-arg:,fcolor-diagnostics,file-prefix:,fix,header-filter:,output-dir:,quiet,system-headers,use-color,use-shell-color,warnings-as-errors: -n "$0" -- "$@"`

if [ $? != 0 ] ; then
  echo "Terminating..." >&2 ;
  usage ;
  exit 1 ; 
fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"


checks=
config_file="--config-file=./config/clang-tidy/.clang-tidy"
excludes=
export_fixes=
extra_arg=
file_prefix=
fix=
header_filter=
includes=
output_dir="_results/clang-tidy"
quiet=
system_headers=
unbuffer=
use_color=
use_shell_color=false
warnings_as_errors=


while true; do
  case "$1" in
         --checks )              checks="--check=$2";                           shift 2 ;;
         --config-file )         config_file="--config-file=$2";                shift 2 ;;
         --export-fixes )        export_fixes="--export-fixes=$2";              shift 2 ;;
         --extra-arg )           extra_arg+=" --extra-arg=$2";                  shift 2 ;;
         --file-prefix )         file_prefix=$2;                                shift 2 ;;
         --fix )                 fix="--fix";                                   shift ;;
         --header-filter )       header_filter="--header-filter=$2";            shift 2 ;;
    -I )                         includes+=" -I $2";                            shift 2 ;;
    -i )                         excludes+=" -isystem $2";                      shift 2 ;;
    -o | --output-dir )          output_dir=$2;                                 shift 2 ;;
         --quiet )               quiet="--quiet";                               shift ;;
         --system-headers )      system_headers="--system-headers";             shift ;;
         --use-color )           use_color="--use-color";                       shift ;;
         --use-shell-color )     use_shell_color=true; unbuffer="unbuffer";     shift ;;
         --warnings-as-errors )  warnings_as_errors="--warnings-as-errors=$2";  shift 2 ;;
         -- )                                                                   shift;      break ;;
    * )                          echo "Unknown option '$1' found !" ; usage ;               break ;;
  esac
done


if [ -z "$output_dir" ]; then
  usage
fi

if [ ! -z "$file_prefix" ]; then
  output_dir+="/$file_prefix"
else
  file_prefix="clang-tidy"
fi

if [ ! -d "$output_dir" ]; then
  mkdir -p $output_dir
fi


echo ""
echo "checks             : $checks"
echo "config-file        : $config_file"
echo "excludes           : $excludes"
echo "export-fixes       : $export_fixes"
echo "extra-arg          : $extra_arg"
echo "file-prefix        : $file_prefix"
echo "fix                : $fix"
echo "header-filter      : $header_filter"
echo "includes           : $includes"
echo "output-dir         : $output_dir"
echo "quiet              : $quiet"
echo "system-headers     : $system_headers"
echo "use-color          : $use_color"
echo "use-shell-color    : $use_shell_color"
echo "warnings-as-errors : $warnings_as_errors"
echo ""


fileReturnValue=0
returnValue=0


for pattern in $*; do
    file_list=`find $pattern -regextype egrep -regex '.*\.(c|cpp|h|hpp)'`

    for file in $file_list; do
        file_base=`basename $file`

        $unbuffer clang-tidy $checks $config_file $export_fixes $extra_arg $fix $header_filter $quiet $system_headers $use_color $warnings_as_errors $file -- $excludes $includes 2>&1 | tee $output_dir/$file_prefix.$file_base.log

        fileReturnValue=${PIPESTATUS[0]}

        if [ $fileReturnValue != 0 ]; then
            returnValue=2
        fi

    done
done


chown -R --reference=`dirname $*` $output_dir/..

echo "$0 done."
exit $returnValue
