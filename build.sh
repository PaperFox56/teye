#!/bin/bash

## Build the given package and execute it if necessary

if $1 ; then
	echo Please enter the name of the package to be build
	exit 1
fi

PACKAGE=$1
SOURCES="src/$PACKAGE.c src/teye.c src/timer.c"

echo Compiling sources $SOURCES to binary $PACKAGE...
gcc $SOURCES -o bin/$PACKAGE
echo Done

if [ "$2" == "r" ] ; then
	./bin/$PACKAGE
else 
	exit 0
fi