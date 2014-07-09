#!/usr/bin/python

#-------------------------------------------------------------------------------
# strip_trailing_whitespace.py
#-------------------------------------------------------------------------------
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#
#-------------------------------------------------------------------------------
# Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
#-------------------------------------------------------------------------------
#
#
# Description:
#
# Edits all source files in given directory and subdirectories so that
# all trailing whitespace is removed.
#
import os
import sys


def pick_endline(line):
	if line.endswith("\r\n"):
		return "\r\n"
	elif line.endswith("\r"):
		return "\r"
	else:
		return "\n"


def process_line(line):
	endline = pick_endline(line)
	return line.rstrip() + endline


def process_file(f):
	lines = []
	for line in f:
		lines.append(process_line(line))
	return "".join(lines)


def process_filepath(fpath):
	if fpath.endswith(".h") or fpath.endswith(".cpp") or fpath.endswith(".c"):
		print >>sys.stderr, fpath
		with open(fpath, "rb") as f:
			output = process_file(f)
		with open(fpath, "wb") as f:
			f.write(output)


rootdir = sys.argv[1]
for root, dirs, files in os.walk(rootdir):
	for fname in files:
		process_filepath(os.path.join(root, fname))
