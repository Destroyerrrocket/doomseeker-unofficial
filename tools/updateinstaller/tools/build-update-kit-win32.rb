#!/usr/bin/ruby

#------------------------------------------------------------------------------
# build-update-kit-win32.rb
#
# Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
#------------------------------------------------------------------------------

# Instructions of use:
# 
# This script is a part of Doomseeker update packages auto-build system.
# Following requirements must be met for this script to work correctly:
# - CreatePackagesScriptName must exist in current working directory and its
#   requirements must be met.
# - PackageConfigsDir must exist and contain at least one .js file that is
#   compliant with config-template.js
# - Doomseeker binary packages need to be compiled beforehand and placed in
#   a directory in a structure that is the same as after deployment on end-user
#   system. All Doomseeker requirements and peripheral files must also be
#   located in this directory (translations, plugins, runtime DLLs, etc.).
# - Optionally, a "update-info_<Platform>.js" may also be located in current
#   working directory. The script will update this file.
#
# Usage:
# 1. Adjust package revisions and version names. Add new packages if necessary.
#    Packages names need to be consistent:
#      a) All names must be lower-case.
#      b) Plugins must be prefixed with "p-".
#      c) Plugin names musn't contain spaces; Doomseeker plugin loader removes
#         any spaces for internal usage.
#      d) Single .js configuration - single package. Mendeley updater allows
#         to define more than one package in a single configuration file but
#         Doomseeker doesn't.
# 2. Call pattern & arguments:
#      script.rb -c <channel> -i <binary-dir>
#
#    -c <channel>
#        Update channel for which the packages will be generated. It must be
#        either "stable" or "beta". This is case-sensitive.
#    -i <binary-dir>
#        Directory where doomseeker.exe resides.

require 'fileutils'
require 'optparse'
require 'json'
require 'securerandom'
require 'date'

###############################################################################
# Configuration Data
###############################################################################
# TODO: Move these configuration settings out to a separate file
# so that the script can be reused on different platforms.
Platform = "win32"
PackageConfigsDir = "win32-configs"
CreatePackagesScriptName = "create-packages-win32.rb"
UrlBase = "http://doomseeker.drdteam.org/updates/"

ChannelStable = {
    "doomseeker" => {
        "revision" => 1,
        "display-version" => "1.0.1",
    },
    "p-chocolatedoom" => {
        "revision" => 1
    },
    "p-odamex" => {
        "revision" => 1
    },
    "p-skulltag" => {
        "revision" => 1
    },
    "p-vavoom" => {
        "revision" => 1
    },
    "p-zandronum" => {
        "revision" => 1
    },
    "p-zdaemon" => {
        "revision" => 1
    },
}

ChannelBeta = {
    "doomseeker" => {
        "revision" => 2,
        "display-version" => "1.0.1",
    },
    "p-chocolatedoom" => {
        "revision" => 1
    },
    "p-odamex" => {
        "revision" => 1
    },
    "p-skulltag" => {
        "revision" => 1
    },
    "p-vavoom" => {
        "revision" => 1
    },
    "p-zandronum" => {
        "revision" => 1
    },
    "p-zdaemon" => {
        "revision" => 1
    },
}

Channels = {
    "stable" => ChannelStable,
    "beta" => ChannelBeta
}

###############################################################################
# Functions
###############################################################################
def read_and_parse_cfg_file(cfg_file_path)
    contents = File.read(cfg_file_path)
    return JSON.parse(contents)
end

def get_package_name_from_config(cfg)
    packages = cfg["packages"]
    if packages == nil || packages.empty?
        raise Exception("Invalid config file (no packages).")
    elsif packages.length > 1
        raise Exception("More than one package is defined.")
    end
    name = packages.first[0]
    return name
end

def spawn_unique_dir(prefix)
    # There's a slight possibility that the directory will already exist,
    # however with UUID and time stamp the risk should be minimal.
    stamp = DateTime.now.strftime("%Y%m%d-%H%M%S")
    name = "#{prefix}-#{stamp}-#{SecureRandom.uuid.slice(0,8)}"
    raise "Directory #{name} already exists." if Dir.exists?(name)
    Dir.mkdir(name)
    return name
end

def process_config(pkg_name, channel, binary_dir, output_dir)
    cfg_file_path = File.join(PackageConfigsDir, "#{pkg_name}.js")
    channel_data = Channels[channel]
    pkg_data = channel_data[pkg_name]
    revision = pkg_data["revision"]
    display_version = pkg_data.include?("display-version") ? 
        pkg_data["display-version"] : revision
    suffix = "_#{revision}-#{channel}_#{Platform}"
    result = system("ruby #{CreatePackagesScriptName} -p #{Platform} " \
        "-v #{display_version} --suffix='#{suffix}' " \
        "#{binary_dir} #{cfg_file_path} #{output_dir}")
    raise "Package generation failed." if !result
end
###############################################################################
# Script Contents
###############################################################################

# Parse arguments.
target_channel = nil
binary_dir = nil

OptionParser.new do |parser|
    parser.banner = "#{$0} -c <channel> -i <binary_dir>"
    parser.on("-c", "--channel <update channel>",
            "Specifies the update channel for which the update " \
                "packages will be generated. Must be either \"beta\" "\
                "or \"stable\".") do |val|
        target_channel = val
    end
    parser.on("-i", "--input [binary_dir]",
        "Directory with deployed Doomseeker, from which packages " \
            "are created.") do |val|
        binary_dir = val
    end
end.parse!

# Validate and process arguments.
begin
    raise "Channel not specified (use -c option)" if !target_channel
    raise "Binary directory not specified (use -i option)" if !binary_dir
    raise "Binary directory \"#{binary_dir}\" doesn't exist." \
        if !Dir.exists?(binary_dir)
    target_channel.downcase!
    raise "Invalid channel name \"#{target_channel}\".\n" \
        "  Allowed names: #{Channels.keys}" \
            if !Channels.keys.include?(target_channel)
rescue
    $stderr.puts "ERROR: #{$!}"
    $stderr.puts "Use -h for help"
    exit(4)
end

output_dir = spawn_unique_dir("upkgs-#{target_channel}")

# Process configs.
successes = []
failures = []
Channels[target_channel].keys.each do |pkg_name|
    begin
        $stderr.puts "==== Now processing: #{pkg_name}"
        process_config(pkg_name, target_channel, binary_dir, output_dir)
        successes << pkg_name
    rescue
        puts $@, $!
        failures << {
            "name" => pkg_name,
            "error" => $!
        }
    end
end

successes.each do |el|
    $stderr.puts "Success: #{el}"
end

failures.each do |el|
    $stderr.puts "FAILURE: #{el["name"]}: #{el["error"].message}"
end

if !successes.empty?
    puts "Created packages are in directory: #{output_dir}"
else
    Dir.rmdir(output_dir)
end

$stderr.puts failures.empty? ? "ALL SUCCESS!" : "ERRORS ENCOUNTERED!"
exit(failures.empty? ? 0 : 3)
