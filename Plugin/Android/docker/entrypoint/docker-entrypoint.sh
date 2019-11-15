#!/bin/bash
. $(dirname $0)/commands.sh

usage="
$(basename "$0") [-t] [-g] [-h] -- program to build plugin

where:
    -t | --test    build android application for testing plugin
    -g | --gstAnd  use prebuilded libgstreamer_android.so
    -h | --help    show this help text
"

BUILD_TEST=false
COPY_GST_AND_LIB=false

show_help=false
while [[ $# -gt 0 ]]
do
  key="$1"
  case $key in

    -h|--help)    show_help=true
                  ;;

    *)            echo "Unknown argument : " $key
                  show_help=true
                  ;;
  esac
  shift
done

if [ "$show_help" = true ]; then
  echo "$usage"
  exit
fi

copyRepo

buildmray
