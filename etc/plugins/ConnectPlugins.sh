#!/bin/bash

# To connect a plugin, create a link in this directory to a plugin definition in 'definitions' subdirectory.
# To list available plugins, run this script without arguments.

if [ $# -eq 0 ]; then
  echo "Available plugins:"
  for A in `ls definitions/*.plg`; do
        NAME="`basename $A .plg`"
	if [ -L "$NAME.plg" ]; then
		printf "   %-30s [CONNECTED]\n" "$NAME"
	else
		printf "   %-30s\n" "$NAME"
	fi
  done 
  exit
fi

echo "Linking plugins:" $*

if [ "$1" == "all" ]; then
  for A in `ls definitions/*.plg`; do
     NAME="`basename $A .plg`"
     ln -s definitions/$NAME.plg $NAME.plg 
  done
else
  for A in $*; do
    if [ -f definitions/$A.plg ]; then
       ln -s definitions/$A.plg $A.plg
    fi
  done
fi
