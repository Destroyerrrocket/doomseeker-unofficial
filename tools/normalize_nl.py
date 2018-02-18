#!/usr/bin/env python3

#-------------------------------------------------------------------------------
# normalize_nl.py
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
# Copyright (C) 2018 "Zalewa" <zalewapl@gmail.com>
#-------------------------------------------------------------------------------
#
# Walks through all source files and counts lone '\n' and '\r\n' pairs.
# If both are present in the file, picks the one with higher count
# and converts the other to this one.
#
# Usage: $0 <rootdir> <extensions>
#
# <extensions> is a comma-separated list of processed extensions,
# eg. ".cpp,.c,.h,.hpp"
import re
import os
import sys


def process_file(filepath):
	with open(filepath, "rb") as f:
		contents_encoded = f.read()
		text = decode_contents(contents_encoded)
		if text is None:
			print("\tcannot decode", file=sys.stderr)
			return None
		normalized = normalize_text(text)
	if normalized is not None:
		with open(filepath, "wb") as f:
			f.write(normalized.encode("utf-8"))


def normalize_text(text):
	LFNL = re.compile(r"\r\n")
	NL = re.compile(r"(?<!\r)\n")

	lfnl_count = count(text, LFNL)
	nl_count = count(text, NL)
	if lfnl_count > 0 and nl_count > 0:
		print("\tlfnl={}, nl={}".format(lfnl_count, nl_count), file=sys.stderr)
		if lfnl_count > nl_count:
			return NL.sub("\r\n", text)
		else:
			return LFNL.sub("\n", text)
	return None


def decode_contents(contents):
	try:
		return contents.decode("utf-8")
	except UnicodeDecodeError:
		pass
	return None


def count(text, pattern):
	return len(pattern.findall(text))


def has_extension(fname, extensions):
	for extension in extensions:
		if fname.endswith(extension):
			return True
	return False


def main():
	rootdir = sys.argv[1]
	extensions = sys.argv[2].split(",")
	for root, dirs, files in os.walk(rootdir):
		for fname in files:
			if not has_extension(fname, extensions):
				continue
			fpath = os.path.join(root, fname)
			print(fpath, file=sys.stderr)
			process_file(fpath)


if __name__ == "__main__":
	main()
