#!/usr/bin/python
#-*- coding: utf-8 -*-
import csv
import json
import os
import sys
import struct
from copy import copy
from multiprocessing import Pool


class Network(object):
    def __init__(self, addr):
        self.addr = addr

    @property
    def addr_int(self):
        ip = self.addr.split("/")[0]
        components = map(int, ip.split("."))
        return reduce(lambda a,b: (a << 8) + b, components)

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
            elif iprange[0] < merged_range[0] and iprange1 > merged_range[1]:
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


if len(sys.argv) < 4:
    print >>sys.stderr, "Usage: {0} <geolite2_blocks.csv> <geolite2_locations.csv> <doomseeker.db>".format(sys.argv[0])
    exit(2)


scriptdir = os.path.dirname(os.path.realpath(__file__))
with open(os.path.join(scriptdir, "country_iso3.json"), "rb") as f:
    country_mapping = json.loads(f.read())


geolite_path = sys.argv[1]
with open(geolite_path, "rb") as f:
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


geolite_locations_path = sys.argv[2]
with open(geolite_locations_path, "rb") as f:
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


def reduce_block(block):
    geoid, networks = block
    len_before = len(networks)
    sys.stderr.write("geoid {0}, networks {1}\n".format(geoid, len(networks)))
    sys.stderr.flush()
    ipranges = reduce_networks(networks)
    sys.stderr.write("geoid {0} - DONE, networks reduced from {1} to {2}\n".format(
        geoid, len(networks), len(ipranges)))
    sys.stderr.flush()
    return geoid, ipranges


pool = Pool()
try:
    items = list(blocks.iteritems())
    items.sort(key=lambda el: len(el[1]), reverse=True)
    blocks = dict(pool.imap_unordered(reduce_block, items))
finally:
    pool.terminate()
    pool.close()


doomseeker_path = sys.argv[3]
with open(doomseeker_path, "wb") as f:
    f.write("IP2C")
    f.write(struct.pack("<H", 2))  # Version
    num = 1
    for geoid, location in locations.iteritems():
        f.write("{0}\0{1}\0".format(location.name, location.code))
        f.write(struct.pack("<I", len(blocks[geoid])))
        for network in blocks[geoid]:
            f.write(struct.pack("<II", network[0], network[1]))
        num += 1
