#!/bin/bash

#-------------------------------------------------------------------------------
# fixindentation.sh
#-------------------------------------------------------------------------------
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
#-------------------------------------------------------------------------------
# Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
#-------------------------------------------------------------------------------

###
# Name:           fixindentation.sh
# Description:    Converts all occurences of 4 spacebars into a single tab
#                 character in every line of all *.h and *.cpp files in current
#                 working directory and its sub-directories.
#                 This only applies to spacebars found at the beginning of each
#                 line.
##

FILES=`find . -iname "*.h" -o -iname "*.cpp"`

for file in $FILES; do
	count=1
	while true; do
		MD5BEFORE=`md5sum $file | awk '{ print $1; }'`
		echo "[$count]: sed -r s/^\(\\s*\)\"    ([^ ])\"/\\1\\t\\2/ $file"
		sed -i -r s/^\(\\s*\)"    ([^ ])"/\\1\\t\\2/ $file
		MD5AFTER=`md5sum $file | awk '{ print $1; }'`

		echo "$MD5BEFORE -> $MD5AFTER"
		if [ $MD5AFTER == $MD5BEFORE ]; then
			break;
		fi
		count=$(($count+1))
	done
done
