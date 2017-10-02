#!/usr/bin/env python3
# coding: utf-8
#
# GeoLite2 conversion script (c) The Doomseeker Team 2016 - 2017.
#
# This script converts GeoLite2 database from the original CSV format into
# compacted format known to Doomseeker. Compacted format is optimized for faster
# loading and for searching country by IP.
#
# Database can either be already stored in locally accessible filesystem or the
# script can automatically download the newest version directly from MaxMind
# website. Run with '-h' argument for usage details.
#
# ----------------------------------------
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
# ----------------------------------------
#
# GeoLite2 - Database and Contents Copyright (c) 2016 MaxMind, Inc.
#
# GeoLite2 is licensed under the Creative Commons Attribution-ShareAlike 3.0
# Unported License. To view a copy of this license, visit
# http://creativecommons.org/licenses/by-sa/3.0/.
#
import binascii
import csv
import json
import hashlib
import gzip
import os
import sys
import shutil
import struct
import tempfile
import zipfile
from copy import copy
from multiprocessing import Pool
from urllib.request import urlopen


country_mapping = None


class Network(object):
    def __init__(self, addr):
        self.addr = addr

    @property
    def addr_int(self):
        ip = self.addr.split("/")[0]
        components = map(int, ip.split("."))
        ip_as_int = 0
        for component in components:
            ip_as_int = (ip_as_int << 8) + component
        return ip_as_int

    @property
    def mask_int(self):
        maskval = int(self.addr.split("/")[1])
        mask = 0
        for bit in range(32 - maskval):
            mask = (mask << 1) | 1
        return mask ^ 0xffffffff

    @property
    def addr_highest_int(self):
        return self.addr_int | (self.mask_int ^ 0xffffffff)

    @property
    def addr_highest(self):
        ipint = self.addr_highest_int
        return "{0}.{1}.{2}.{3}".format((ipint & 0xff000000) >> 24, (ipint & 0xff0000) >> 16,
                                        (ipint & 0xff00) >> 8, ipint & 0xff)

    def __str__(self):
        return self.addr


class Location(object):
    def __init__(self, geoid, continent_code, continent_name,
                 country_code, country_name):
        self.geoid = geoid
        self.continent_code = continent_code
        self.continent_name = continent_name
        self.country_code = country_code
        self.country_name = country_name

    @property
    def name(self):
        if self.country_name:
            return self.country_name
        else:
            return self.continent_name

    @property
    def code(self):
        if self.country_code:
            return country_mapping[self.country_code]
        else:
            return self.continent_code

    def __str__(self):
        return "{0},{1}".format(self.name, self.code)


def download_db():
    url = "https://geolite.maxmind.com/download/geoip/database/GeoLite2-Country-CSV.zip"
    f = None
    try:
        with tempfile.NamedTemporaryFile(delete=False) as f:
            download_url(url, f)
        tmpdir = tempfile.mkdtemp()
        unzip(f.name, tmpdir)
    finally:
        if f:
            try:
                os.unlink(f.name)
            except Exception as e:
                _prerr("Warning: failed to delete file {0}:\n\t{1}".format(f.name, e))
    return find_geolite_files(tmpdir), tmpdir


def download_url(url, outfile):
    stream = urlopen(url)
    try:
        _prerr("Receiving URL {0}".format(url))
        collected = 0
        while True:
            chunk = stream.read(1024 * 20)
            if not chunk:
                break
            collected += len(chunk)
            _prerr("Downloaded {0} B".format(collected))
            outfile.write(chunk)
    finally:
        stream.close()


def unzip(zippath, outdir):
    _prerr("Extracting downloaded file '{0}'\n\tto directory '{1}'".format(
        zippath, outdir))
    with zipfile.ZipFile(zippath) as zf:
        zf.extractall(outdir)


def find_geolite_files(maindir):
    # GeoLite2 zip contains 1 entry in its root which is a directory with a
    # changing name that contains release date.
    for entry in os.listdir(maindir):
        geolite_dir = os.path.join(maindir, entry)
        break
    geolite_blocks_path = os.path.join(geolite_dir, "GeoLite2-Country-Blocks-IPv4.csv")
    geolite_countries_path = os.path.join(geolite_dir, "GeoLite2-Country-Locations-en.csv")
    paths = (geolite_blocks_path, geolite_countries_path)
    for f in paths:
        if not os.path.isfile(f):
            raise IOError("File '{0}' not found".format(f))
    return paths


def reduce_block(block):
    geoid, networks = block
    sys.stderr.write("geoid {0}, networks {1}\n".format(geoid, len(networks)))
    sys.stderr.flush()
    ipranges = reduce_networks(networks)
    sys.stderr.write("geoid {0} - DONE, networks reduced from {1} to {2}\n".format(
        geoid, len(networks), len(ipranges)))
    sys.stderr.flush()
    return geoid, ipranges


def reduce_networks(networks):
    ipranges = networks_to_ipranges(networks)
    ipranges = merge_consecutive_ipranges(ipranges)
    ipranges = merge_inclusive_ipranges(ipranges)
    ipranges.sort(key=lambda el: el[0])
    return ipranges


def networks_to_ipranges(networks):
    return map(lambda net: [net.addr_int, net.addr_highest_int], networks)


def merge_consecutive_ipranges(ipranges):
    ipranges = sorted(ipranges, key=lambda el: el[0])
    merged = []
    for iprange in ipranges:
        if merged:
            merged_range = merged[-1]
            if iprange[0] - merged_range[1] == 1:
                merged_range[1] = iprange[1]
                continue
            if iprange[1] - merged_range[0] == -1:
                merged_range[0] = iprange[0]
                continue
        merged.append(iprange)
    return merged


def merge_inclusive_ipranges(ipranges):
    ipranges = copy(ipranges)
    merged = []
    for iprange in ipranges:
        skip = False
        for merged_range in merged:
            if iprange[0] > merged_range[0] and iprange[1] < merged_range[1]:
                skip = True
                break
            elif iprange[0] < merged_range[0] and iprange[1] > merged_range[1]:
                merged_range[0] = iprange[0]
                merged_range[1] = iprange[1]
                skip = True
                break
            elif iprange[0] < merged_range[0] and iprange[1] > merged_range[0]:
                merged_range[0] = iprange[0]
                skip = True
                break
            elif iprange[0] > merged_range[0] and iprange[0] < merged_range[1]:
                merged_range[1] = iprange[1]
                skip = True
                break
        if not skip:
            merged.append(iprange)
    return merged


def is_help():
    return "-h" in sys.argv or "--help" in sys.argv


def usage():
    _prerr("Usage: {0} <geolite2_blocks.csv> <geolite2_locations.csv> <doomseeker.db>".format(sys.argv[0]))
    _prerr("   or: {0} <doomseeker.db>  # Auto-downloads GeoLite2 database".format(sys.argv[0]))
    _prerr("   or: {0} <-h|--help>      # Prints help".format(sys.argv[0]))


def convert(geolite_blocks_path, geolite_locations_path, doomseeker_path):
    with open(geolite_blocks_path, "r") as f:
        reader = csv.reader(f)
        blocks = {}
        for rownum, row in enumerate(reader):
            if rownum == 0:
                # Header row.
                continue
            network = Network(row[0])
            geoid = row[1] if row[1] else row[2]
            if not geoid:
                continue
            geoid = int(geoid)
            if geoid in blocks:
                blocks[geoid].append(network)
            else:
                blocks[geoid] = [network]


    with open(geolite_locations_path, "r") as f:
        reader = csv.reader(f)
        locations = {}
        for rownum, row in enumerate(reader):
            if rownum == 0:
                # Header row
                continue
            geoid = int(row[0])
            location = Location(geoid=geoid, continent_code=row[2],
                                continent_name=row[3],
                                country_code=row[4], country_name=row[5])
            locations[geoid] = location


    pool = Pool()
    try:
        items = list(blocks.items())
        items.sort(key=lambda el: len(el[1]), reverse=True)
        blocks = dict(pool.imap_unordered(reduce_block, items))
    finally:
        pool.terminate()
        pool.close()


    with open(doomseeker_path, "wb") as f:
        f.write(b"IP2C")
        f.write(struct.pack(b"<H", 2))  # Version
        num = 1
        for geoid, location in locations.items():
            f.write(location.name.encode("utf-8") + b"\0")
            f.write(location.code.encode("utf-8") + b"\0")
            f.write(struct.pack(b"<I", len(blocks[geoid])))
            for network in blocks[geoid]:
                f.write(struct.pack(b"<II", network[0], network[1]))
            num += 1


def md5(fpath):
    with open(fpath, "rb") as f:
        checksum = hashlib.md5(f.read()).digest()
        _prerr("MD5: {0}".format(binascii.hexlify(checksum).lower()))


def gzip_file(fpath):
    outpath = "{0}.gz".format(fpath)
    with gzip.open(outpath, "wb") as gfile:
        with open(fpath, "rb") as f:
            gfile.write(f.read())
    _prerr("GZip file generated at: {0}".format(outpath))


def run():
    if is_help():
        usage()
        exit(0)

    scriptdir = os.path.dirname(os.path.realpath(__file__))
    with open(os.path.join(scriptdir, "country_iso3.json"), "r") as f:
        global country_mapping
        country_mapping = json.loads(f.read())

    tmpdir = None
    try:
        if len(sys.argv) == 2:
            geolite_files, tmpdir = download_db()
            geolite_blocks_path, geolite_locations_path = geolite_files
            doomseeker_path = sys.argv[1]
        elif len(sys.argv) == 4:
            geolite_blocks_path = sys.argv[1]
            geolite_locations_path = sys.argv[2]
            doomseeker_path = sys.argv[3]
        else:
            usage()
            exit(2)

        convert(geolite_blocks_path, geolite_locations_path, doomseeker_path)
    finally:
        if tmpdir:
            try:
                shutil.rmtree(tmpdir)
            except Exception as e:
                _prerr("Warning: failed to clean up tmpdir '{0}': {1}".format(tmpdir, e))
    md5(doomseeker_path)
    gzip_file(doomseeker_path)


def _prerr(*args, **kwargs):
    kwargs.setdefault("file", sys.stderr)
    print(*args, **kwargs)


if __name__ == "__main__":
    run()
