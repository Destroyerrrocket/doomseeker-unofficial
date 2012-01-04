#!/bin/bash

# Get the version number
version_info=`grep --only-matching -E '[0-9.]{3,}|Beta|Alpha' ../src/core/version.cpp`
ws_version=`grep -E 'return "[0-9.]+"' ../src/wadseeker/wadseekerversioninfo.cpp | grep --only-matching -E '[0-9.]{3,}|Beta|Alpha'`
version="0.0"
tag=""
for i in $version_info
do
	if [ $i == "Beta" ]
	then
		tag="b"
	elif [ $i == "Alpha" ]
	then
		tag="a"
	else
		version=$i
	fi
done

echo "Version: $version"
echo "Tag: $tag"
echo "Wadseeker Version: $ws_version"
echo

# Generate packages
ds_package=doomseeker-${version}${tag}_src
ws_package=libwadseeker-${ws_version}_src

# makePackage <packagename> <srcDir>
function makePackage()
{
	echo "Generating $1.tar.bz2"
	ln -s $2 $1
	tar cf $1.tar $1/*  --exclude=.svn --exclude=build --exclude=releasescripts --exclude=*~
	bzip2 -9 $1.tar
	rm $1
}

makePackage $ds_package ..
makePackage $ws_package ../src/wadseeker
