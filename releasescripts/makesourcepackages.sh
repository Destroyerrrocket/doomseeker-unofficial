#!/bin/bash
#------------------------------------------------------------------------------
# makesourcepackages.sh
#------------------------------------------------------------------------------
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301  USA
#
#------------------------------------------------------------------------------
# Copyright (C) 2010 Braden "Blzut3" Obrzut <admin@maniacsvault.net>
#------------------------------------------------------------------------------

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
