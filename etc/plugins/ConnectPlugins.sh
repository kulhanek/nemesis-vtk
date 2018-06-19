#!/bin/bash

# To connect a plugin, create a link in this directory to a plugin definition in 'definitions' subdirectory.
# To list available plugins, run this script without arguments.

if [ $# -eq 0 ]; then
  echo "Available plugins:"
  for NAME in `find definitions -name '*.plg' | sort`; do
    BASE=`basename $NAME .plg`
	if [ -L "$BASE.plg" ]; then
		printf "   %-60s [CONNECTED]\n" "$NAME"
	else
		printf "   %-60s\n" "$NAME"
	fi
  done 
  exit
fi

echo "Linking plugins:" $*

if [ "$1" == "all" ]; then
  for A in `find definitions -name '*.plg'`; do
     NAME="`basename $A .plg`"
     ln -s $A $NAME.plg 2> /dev/null
  done
else
  for A in $*; do
    NAME=`find definitions -name "${A}.plg"`
    if [ -f "$NAME" ]; then
       ln -s "$NAME" $A.plg
    fi
  done
fi
