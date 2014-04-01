#!/usr/bin/python

#-------------------------------------------------------------------------------
# fix_excessive_whitespace.py
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
# Name:           fix_excessive_whitespace.py
# Description:    Converts this:
#                 void            something();
#                 info this:
#                 void something();
#                 Basically, removes excessive whitespace between two
#                 printable characters. Omits comment blocks.
#                 Affects only .h files.
#
import re
import os
import sys


def is_pos_in_comment(line, pos):
	return line.find("//") >= 0 and pos > line.find("//")


def process_pattern(line, regex):
	pattern = re.compile(regex)
	match = pattern.search(line)
	if match:
		if not is_pos_in_comment(line, match.start()):
			return "{0}{1} {2}{3}".format(
				line[:match.start()],
				match.group(1),
				match.group(2),
				line[match.end():]
			)
	return line


def process_excessive_whitespace(line):
	return process_pattern(line, r"(\S)(?:\s{2,})(\S)")


def process_tabs_between_keywords(line):
	return process_pattern(line, r"(\S)\t(\S)")


def process_line(line):
	line = process_excessive_whitespace(line)
	line = process_tabs_between_keywords(line)
	return line


def process_file(f):
	lines = []
	block_comment = False
	for line in f:
		idx_block_comment = line.find("/*")
		if idx_block_comment >= 0:
			block_comment = True
		if block_comment:
			if line.find("*/") > idx_block_comment:
				block_comment = False
			lines.append(line)
			continue
		lines.append(process_line(line))
	return "".join(lines)


rootdir = sys.argv[1]
for root, dirs, files in os.walk(rootdir):
	for fname in files:
		if not fname.endswith(".h"):
			continue
		fpath = os.path.join(root, fname)
		print >>sys.stderr, fpath
		with open(os.path.join(root, fname), "rb") as f:
			output = process_file(f)
		with open(os.path.join(root, fname), "wb") as f:
			f.write(output)
