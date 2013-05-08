#!/usr/bin/ruby

#------------------------------------------------------------------------------
# build-update-kit-win32.rb
#
# Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
#------------------------------------------------------------------------------

# Instructions of use:
#
# This script is a part of Doomseeker update packages auto-build system.
#
# What this does:
# 1. For every package that is defined in CHANNELS[<channel>] hash there is
#    a call to CREATE_PACKAGE_SCRIPT_NAME script. Platform, display version and
#    suffix information is passed to this script as arguments.
# 2. An output directory is determined basing on current timestamp and randomly
#    generated string of 8 characters. This directory is created in current
#    working directory.
# 3. CREATE_PACKAGE_SCRIPT_NAME script creates two files in the output
#    directory. One file is a package .zip archive, the other is a Mendeley
#    updater's XML script. Files are named basing on internal package name,
#    revision, channel and platform.
# 4. If at least one package was created successfully, this script will
#    create an "update-info.js" file. This file contains information on
#    all packages for all channels. Doomseeker downloads this file each time
#    when it wants to check if there are any new updates. OVERWRITE THIS FILE
#    ON THE SERVER WITH ABSOLUTE CARE!!! The file is formatted in
#    a human-readable fashion for purpose. Always double-check if revisions for
#    all channels are correct, otherwise you may screw up people's setups.
#
# Requirements:
# Following requirements must be met for this script to work correctly:
# - "ruby" executable must be in ENV["PATH"].
# - CREATE_PACKAGE_SCRIPT_NAME must exist in current working directory and its
#   requirements must be met.
# - PACKAGE_CONFIGS_DIR must exist and contain .js files, one for each package.
#   .js files must be compliant with config-template.js. Files must be named
#   after internal package names (doomseeker.js, p-chocolatedoom.js, etc.).
# - Doomseeker binary packages need to be compiled beforehand and placed in
#   a directory in a structure that is the same as after deployment on end-user
#   system. All Doomseeker requirements and peripheral files must also be
#   located in this directory (translations, plugins, runtime DLLs, etc.).
#   `make install`, or INSTALL target in Visual Studio, should take care of
#   creating appropriate structure.
#
# Usage:
# 1. Adjust package revisions and version names. Add new packages if necessary.
#    Packages names need to be consistent:
#      a) All names must be lower-case.
#      b) Plugins must be prefixed with "p-".
#      c) Plugin names musn't contain spaces; Doomseeker plugin loader removes
#         any spaces for internal usage.
# 2. Call pattern & arguments:
#      script.rb -c <channel> -i <binary-dir>
#
#    -c <channel>
#        Update channel for which the packages will be generated. It must be
#        either "stable" or "beta". This is case-sensitive.
#    -i <binary-dir>
#        Directory where doomseeker.exe resides.
# 3. Commit this file when revision information changes.

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
# TODO: The version information could be read from the source-code, provided
# that this file is not moved from the repo, and provided that the version
# information in the code actually matches the version information in
# the binary that is about to be packaged.
PLATFORM = "win32"
PACKAGE_CONFIGS_DIR = "win32-configs"
CREATE_PACKAGE_SCRIPT_NAME = "create-packages-win32.rb"
URL_BASE = "http://doomseeker.drdteam.org/updates/"

DISPLAY_NAMES = {
    "doomseeker" => "Doomseeker",
    "p-chocolatedoom" => "Chocolate Doom",
    "p-odamex" => "Odamex",
    "p-skulltag" => "Skulltag",
    "p-vavoom" => "Vavoom",
    "p-zandronum" => "Zandronum",
    "p-zdaemon" => "ZDaemon"
}

CHANNEL_STABLE = {
    "doomseeker" => {
        "revision" => <insert-revision-here>,
        "display-version" => "0.10 Beta",
    },
    "p-chocolatedoom" => {
        "revision" => 6
    },
    "p-odamex" => {
        "revision" => 11
    },
    "p-skulltag" => {
        "revision" => 12
    },
    "p-vavoom" => {
        "revision" => 5
    },
    "p-zandronum" => {
        "revision" => 2
    },
    "p-zdaemon" => {
        "revision" => 5
    },
}

CHANNEL_BETA = {
    "doomseeker" => {
        "revision" => <insert-revision-here>,
        "display-version" => "0.10 Beta",
    },
    "p-chocolatedoom" => {
        "revision" => 6
    },
    "p-odamex" => {
        "revision" => 11
    },
    "p-skulltag" => {
        "revision" => 12
    },
    "p-vavoom" => {
        "revision" => 5
    },
    "p-zandronum" => {
        "revision" => 2
    },
    "p-zdaemon" => {
        "revision" => 5
    },
}

CHANNELS = {
    "stable" => CHANNEL_STABLE,
    "beta" => CHANNEL_BETA
}

###############################################################################
# Functions
###############################################################################
def spawn_unique_dir(prefix)
    # There's a slight possibility that the directory will already exist,
    # however with UUID and time stamp the risk should be minimal.
    stamp = DateTime.now.strftime("%Y%m%d-%H%M%S")
    name = "#{prefix}-#{stamp}-#{SecureRandom.uuid.slice(0,8)}"
    raise "Directory #{name} already exists." if Dir.exists?(name)
    Dir.mkdir(name)
    return name
end

def package_suffix(revision, channel, platform)
    return "_#{revision}-#{channel}_#{platform}"
end

def package_filename(pkg_name, revision, channel, platform)
    return "#{pkg_name}#{package_suffix(revision, channel, platform)}.zip"
end

def extract_display_version(pkg_data)
    return pkg_data.include?("display-version") ?
        pkg_data["display-version"] : pkg_data["revision"].to_s
end

def process_package(pkg_name, channel, binary_dir, output_dir)
    # Create the package archive and .xml script by calling
    # the "create-packages" script.
    
    # Extract necessary information on the package.
    channel_data = CHANNELS[channel]
    pkg_data = channel_data[pkg_name]
    revision = pkg_data["revision"]
    display_version = extract_display_version(pkg_data)
    suffix = package_suffix(revision, channel, PLATFORM)
    # Get path to the .js config file required by the script.
    cfg_file_path = File.join(PACKAGE_CONFIGS_DIR, "#{pkg_name}.js")
    # Run script.
    result = system("ruby", CREATE_PACKAGE_SCRIPT_NAME, "-p", PLATFORM,
        "-v", display_version, "--suffix", suffix,
        binary_dir, cfg_file_path, output_dir)
    raise "Package generation failed." if !result
end

def dump_update_info(output_path)
    # Convert internal channel information to data understood by Doomseeker's
    # update-info parser.
    update_info = {}
    CHANNELS.each do |channel_name, channel_data|
        channel_data.each do |pkg, pkg_info|
            update_info[pkg] = {} if !update_info.include?(pkg)
            revision = pkg_info["revision"]
            filename = package_filename(pkg, revision, channel_name, PLATFORM)
            url = File.join(URL_BASE, filename)
            update_info[pkg][channel_name] = {
                "revision" => revision,
                "display-version" => extract_display_version(pkg_info),
                "display-name" => DISPLAY_NAMES[pkg],
                "URL" => url
            }
        end
    end
    json_data = JSON.pretty_generate(update_info)
    File.open(output_path, "w") do |f|
        f.write(json_data)
        f.write("\n")
    end
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
        "  Allowed names: #{CHANNELS.keys}" \
            if !CHANNELS.keys.include?(target_channel)
rescue
    $stderr.puts "ERROR: #{$!}"
    $stderr.puts "Use -h for help"
    exit(4)
end

output_dir = spawn_unique_dir("upkgs-#{target_channel}")

# Process packages.
successes = []
failures = []
CHANNELS[target_channel].keys.each do |pkg_name|
    begin
        $stderr.puts "==== Now processing: #{pkg_name}"
        process_package(pkg_name, target_channel, binary_dir, output_dir)
        successes << pkg_name
    rescue
        puts $@, $!
        failures << {
            "name" => pkg_name,
            "error" => $!
        }
    end
end

# Print success/failure states.
successes.each do |el|
    $stderr.puts "Success: #{el}"
end

failures.each do |el|
    $stderr.puts "FAILURE: #{el["name"]}: #{el["error"].message}"
end

if !successes.empty?
    $stderr.puts "Created packages are in directory: #{output_dir}"
    # If at least one package was successful create the update-info.js file.
    update_info_path = File.join(output_dir, "update-info_#{PLATFORM}.js")
    $stderr.puts "Creating update info file: #{update_info_path}"
    dump_update_info(update_info_path)
else
    # Delete the output directory if all packages failed.
    begin
        Dir.rmdir(output_dir)
    rescue
        puts $@, $!
    end
end

$stderr.puts failures.empty? ? "ALL SUCCESS!" : "ERRORS ENCOUNTERED!"
exit(failures.empty? ? 0 : 3)
