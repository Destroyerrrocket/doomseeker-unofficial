#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: `basename $0` <doxyconffile>"
	exit 1
fi

doxyfile=$1
tmpdoxyfile="/tmp/doomseekerdoc.doxycfg"
outputdir="/tmp/doomseekerdoc"
scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
versiondefs="$scriptdir/src/core/versiondefs.h"

version=`cat "$versiondefs" | grep VERSION_STRING | tr -s " " | cut -d" " -f3-`
cp "$1" "$tmpdoxyfile"
sed -i -r 's/PROJECT_NUMBER\s*=.*/PROJECT_NUMBER = '"$version"'/' $tmpdoxyfile
sed -i -r 's:OUTPUT_DIRECTORY\s*=.*:OUTPUT_DIRECTORY = '"$outputdir"':' $tmpdoxyfile

mkdir /tmp/doomseekerdoc
doxygen "$tmpdoxyfile"
exitcode=$?
if [ $exitcode -eq 0 ]; then
	rm "$tmpdoxyfile"
	echo "Doc created at '$outputdir'"
else
	exit $exitcode
fi

