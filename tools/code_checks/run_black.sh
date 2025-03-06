#!/bin/bash


echo "executing $0 $* ..."


usage() {
  echo "Usage: $0 :" 1>&2;
  echo "          [--check] <Process, but do not make any changes.>  [--color] <Coloured output.>  [--config] <Config file.>  [--diff] <Show diff.>  [--file-prefix] <File_prefix>  [-o | --output-dir] <output directory>" 1>&2;
  echo "          [--quiet] <Only critical output is emitted.>  [--use-shell-color] <Use color in terminal.>  [--verbose] <Show verbose output.>" 1>&2;
  exit 1;
}


# This requires GNU getopt.
TEMP=`getopt -o o:qv --long check,color,config:,diff,file-prefix:,output-dir:,quiet,use-shell-color,verbose -n "$0" -- "$@"`

if [ $? != 0 ] ; then
  echo "Terminating..." >&2 ;
  usage ;
  exit 1 ; 
fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"


check=
color=
diff=
file_prefix="black"
output_dir="_results/black"
quiet=
unbuffer=
use_shell_color=false
verbose=


while true; do
  case "$1" in
         --check )                check="--check";                                  shift ;;
         --color )                color="--color";                                  shift ;;
         --config )               config="--config $2";                             shift 2 ;;
         --diff )                 diff="--diff";                                    shift ;;
         --file-prefix )          file_prefix=$2;                                   shift 2 ;;
    -o | --output-dir )           output_dir=$2;                                    shift 2 ;;
    -q | --quiet )                quiet="--quiet";                                  shift ;;
         --use-shell-color )      use_shell_color=true; unbuffer="unbuffer";        shift ;;
    -v | --verbose )              verbose="--verbose";                              shift ;;
         -- )                                                                       shift;      break ;;
    * )                           echo "Unknown option '$1' found !" ; usage ;                  break ;;
  esac
done


echo ""
echo "check            : $check"
echo "color            : $color"
echo "config           : $config"
echo "diff             : $diff"
echo "file-prefix      : $file_prefix"
echo "output-dir       : $output_dir"
echo "quiet            : $quiet"
echo "use-shell-color  : $use_shell_color"
echo "verbose          : $verbose"
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
    file_list=`find $pattern -regextype egrep -regex '.*\.(py)'`

    for file in $file_list; do
        file_base=`basename $file`

        echo "$unbuffer black $check $color $config $diff $quiet $verbose $file 2>&1 | tee $output_dir/$file_prefix.$file_base.log"
        $unbuffer black $check $color $config $diff $quiet $verbose $file 2>&1 | tee $output_dir/$file_prefix.$file_base.log
        
        fileReturnValue=${PIPESTATUS[0]}

        if [ $fileReturnValue != 0 ]; then
            returnValue=2
        fi

    done
done


chown -R --reference=`dirname $*` $output_dir

echo "$0 done."
exit $returnValue
