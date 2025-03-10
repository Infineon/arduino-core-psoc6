#!/bin/bash


echo "executing $0 $* ..."


usage() {
  echo "Usage: $0 :" 1>&2;
  echo "          [-i] <Edit in-place.>  [-n | --dry-run] <Do not make any changes.>  [--fcolor-diagnostics] <Coloured output.>  [--files] <File list.>  [--file-prefix] <File_prefix>  [-o | --output-dir] <output directory>" 1>&2;
  echo "          [--output-replacements-xml] <Replacement XML.>  [ --sort-includes] <Sort includes.>  [--style] <Style or config file.>  [--use-shell-color] <Use color in terminal.>  [--verbose] <Show list of processed files.>  [ --Wno-error] <Do not error out for named warnings.>  [--Werror]  <Upgrade warnings to errors.> " 1>&2;
  exit 1;
}


# This requires GNU getopt.
TEMP=`getopt -o ino: --long dry-run,fcolor-diagnostics,files:,file-prefix:,output-dir:,output-replacements-xml:,sort-includes,style:,use-shell-color,verbose,Wno-error:,Werror: -n "$0" -- "$@"`

if [ $? != 0 ] ; then
  echo "Terminating..." >&2 ;
  usage ;
  exit 1 ; 
fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"


dry_run=
edit_in_place="-i"
fcolor_diagnostics="--fcolor-diagnostics"
files=
file_prefix="clang-format"
output_dir="_results/clang-format"
output_replacements_xml=
sort_includes="--sort-includes"
style="-style=file:"./config/clang-format/.clang-format""
unbuffer=
use_shell_color=false
verbose="--verbose"
wno_error=
werror=--Werror


while true; do
  case "$1" in
    -n | --dry-run )                  dry_run="--dry-run";                                     shift ;;
         --fcolor-diagnostics )       fcolor_diagnostics="--fcolor-diagnostics";               shift ;;
         --files )                    files="--files=$2";                                      shift 2 ;;
         --file-prefix )              file_prefix=$2;                                          shift 2 ;;
    -i )                              edit_in_place="-i";                                      shift ;;
    -o | --output-dir )               output_dir=$2;                                           shift 2 ;;
         --output-replacements-xml )  output_replacements_xml="--output-replacements-xml=$2";  shift 2 ;;
         --sort-includes )            sort_includes="--sort-includes";                         shift ;;
         --style )                    style="--style=$2";                                      shift 2 ;;
         --use-shell-color )          use_shell_color=true; unbuffer="unbuffer";               shift ;;
         --verbose )                  verbose="--verbose";                                     shift ;;
         --Wno-error )                wno_error="--Wno-error=$2";                              shift 2 ;;
         --Werror )                   werror="--Werror";                                       shift ;;
         -- )                                                                                  shift;      break ;;
    * )                               echo "Unknown option '$1' found !" ; usage ;                         break ;;
  esac
done


echo ""
echo "dry-run                 : $dry_run"
echo "fcolor-diagnostics      : $fcolor_diagnostics"
echo "files                   : $files"
echo "file-prefix             : $file_prefix"
echo "i (edit in-place)       : $edit_in_place"
echo "output-dir              : $output_dir"
echo "output-replacements-xml : $output_replacements_xml"
echo "sort-includes           : $sort_includes"
echo "style                   : $style"
echo "use-shell-color         : $use_shell_color"
echo "verbose                 : $verbose"
echo "Wno-error               : $wno_error"
echo "Werror                  : $werror"
echo ""


if [ -z "$output_dir" ]; then
  usage
fi

if [ ! -d "$output_dir" ]; then
  mkdir -p $output_dir
fi


fileReturnValue=0
returnValue=0

for pattern in $*; do
    file_list=`find $pattern -regextype egrep -regex '.*\.(c|cpp|h|hpp)'`

    for file in $file_list; do
        file_base=`basename $file`

        # echo "$unbuffer clang-format $dry_run $edit_in_place $fcolor_diagnostics $files $output_replacements_xml $sort_includes $style $verbose $wno_error $werror $file 2>&1 | tee $output_dir/$file_prefix.$file_base.log"
        # new option in v21.x : --fail-on-incomplete-format 
        $unbuffer clang-format $dry_run $edit_in_place $fcolor_diagnostics $files $output_replacements_xml $sort_includes $style $verbose $wno_error $werror $file 2>&1 | tee $output_dir/$file_prefix.$file_base.log

        fileReturnValue=${PIPESTATUS[0]}

        if [ $fileReturnValue != 0 ]; then
            returnValue=2
        fi

    done
done


chown -R --reference=`dirname $*` $output_dir

echo "$0 done."
exit $returnValue
