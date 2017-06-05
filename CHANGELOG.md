# Change Log

Visit this file cleanly formatted in repository:
<https://bitbucket.org/Doomseeker/doomseeker/src/default/CHANGELOG.md>

This file follows recommendations in accordance to:
<http://keepachangelog.com/>

## [1.1] - 2017-06-05
### Added
- Provide more information in game status widgets (bottom-right corner).
Humans and bots are now counted separately and together, refresh progress
is included and the numbers also encompass custom and LAN servers.
- Provide more statistic information in Doomseeker's tray icon's tooltip.
Tooltip is also refreshed live during server list refresh (it might
require moving cursor away from the tray icon depending on platform.)
- Human-readable versions in update packages file names.
- Redistribute OpenSSL libraries together with Doomseeker.
- Servers can be filtered by the "testing server" flag.
- Use global game custom parameters when hosting a server or creating
offline game. These parameters are used in addition to parameters
defined in "Custom Parameters" tab in Create Game dialog.
- Support varying executables for each game instead of just "game.exe"
or "client/server.exe". Chocolate Doom uses different executables
depending on IWAD and Doomseeker now supports that.
- Suggest different game executables in Create Game dialog depending
on what the plugin for each game declares. Executables can be picked
from a drop-down list. Zandronum testing executables should also be on
this list.
- Added "Install Freedoom" dialog box to main menu (under "File").
- Apply default values to game limits in Create Game box
if engine plugin provides them.
- Log plugin load failure when ABI version mismatches (when plugin is too old).
- Added "play sound" button to IRC sounds configuration page.
- Added UPnP checkbox to "Create Game" box. Works with Odamex and Zandronum.
- Paint 'T' icon over game logos for testing servers.
- IRC: Send NOTICEs to channel or priv chat box if such box is open,
send them to network tab if not.
- IRC: Consider reply code 338 (RPL_WHOISACTUALLY) a part of /whois
response.
- IRC: Parse response codes RPLChannelUrl (328) and
RPLCreatedTime(329) and forward them to originating channel's
window.
- IRC: Parse channel related errors 471-477 (bad password, banned,
etc.) and forward some of them to current chat box.
- Windows: Server list refresh progress and Wadseeker download progress
are now shown in Windows' task bar.
- Wadseeker GUI: Added option to always use Wadseeker's hardcoded list
of sites as it exists in the currently used Wadseeker version.
With Wadseeker updates new sites may come and old (defunct) sites may go.
This option is on by default.
- Wadseeker: Added "Fetch Freedoom version info" feature to
WADSEEKER_API.
- Wadseeker: Added ModInstall class to WADSEEKER_API for mods with
known URLs.
- Wadseeker: Queries to services (idgames & wadarchive) are shown in
the table with site queries.
- Wadseeker: Sites and services (idgames & wadarchive) queries can
be aborted by the user.
- Wadseeker: install files from archive's subdirectories if archive's root
doesn't have them. This indirectly fixes problem with Freedoom no longer
installing from the "Install Freedoom" dialog box.
- SRB2: New plugin for Sonic Robo Blast 2 - supports game setup,
master server, server queries, IRC channel.
- Zandronum: LAN broadcast servers will now appear on server list.
- Zandronum: "force spectator" voting can be configured from "Create
Game" dialog.
- Zandronum: display DeHackEd patches loaded on servers, allow the
game client to load them when joining.
- Zandronum: support switching between dmflag sets for Zandronum 2.x
and Zandronum 3.x. Allow to choose Zandronum version in "Create Game"
dialog. Understand Zandronum server versions when displaying
dmflags in "Server Details" info box.

### Changed
- Switched from Qt4 to Qt5.
- Windows: Filter files by *.exe, *.com and *.bat when browsing for
either server or client game executables.
- Windows & Mac: Update packages have been split from monolithic
'doomseeker' to smaller packages. Downloading updates won't
require to download Qt libraries everytime anymore.
- Completely reworked "missing files" dialog that appears when
trying to join a server and not having all the necessary files.
- "Find missing WADs" server list context menu action: allow user
to select the files that are to be installed.
- Added "Install Freedoom" button to "missing files" dialog
if Doom IWAD is missing.
- To reduce noise in generated command lines, wrap with quotes only
those arguments that have characters outside of a very narrow, safe
range.
- Use GeoLite2 database for IP2C.
- Restructured how game dmflags are stored to account for the fact
that games sometimes like to change numerical values of the dmflags
while dmflags themselves are effectively the same (ie. same name, same behavior,
just a different code). Doomseeker will now rely on an internal dmflag
name and deduce the value basing on what's currently programmed
into the plugins it uses.
- Server name filter, when non-empty, is now always in effect,
regardless of whether all other server filters are enabled or not.
This allow to search the list for a server name while not having
to enable other filters.
- hexdd.wad can no longer be picked as an IWAD in "Create Game" box.
- Updated internal 7z (LZMA SDK) to version 9.35 (from 4.65).
- Updated internal bzip2 sources to version 1.0.6 (from 1.0.5).
- Improved validation of configuration dialogs. User will be immediately
and clearly notified when Doomseeker detects a configuration problem.
Configuration dialog page tree will show a warning icon next to pages
that report problems. Pages that configure file or directory paths
have been programmed to report validation errors.
- Query speed improvements:
    - Split server querying onto several client sockets, max. 25 host-ports per
      socket. This prevents packets from "getting lost" and servers appearing as
      "not responding" during aggressive queries.
    - Lowered speed limits in "Query" configuration page. Users can now
      configure more aggressive queries (1 attempt per server, 1 second timeout,
      1 millisecond interval between servers).
    - Added "Very Aggressive" query preset that will push it to the limit.
    - "Aggressive" query preset: attempts reduced to 2 (from 3) and interval
      between servers reduced to 5 ms (from 10).
    - Made "Aggressive" query mode the default one for new configs.
- Changed appearance of color buttons to mesh better with OS X.
The altered look affects all OSes.
- Also for OS X purposes, minimally altered the sizes of "browse" buttons.
- Chocolate Doom: Don't show max players/clients and map list
widgets in Create Game box.
- Vavoom: plugin deprecated, discontinued and blacklisted by Doomseeker.
- Zandronum: automatically try to set "Testing binaries" root path
to Doomseeker's "My Documents" directory. This should help new users to start
using Zandronum testing builds without having to configure the path.
- Zandronum: allow to install testing version of the game even if target
directory exists and is non-empty. The message box that pops up when
encountering an unclean directory is slightly different and is displayed
with a warning icon to inform the user that pre-existing files may be
overwritten by the installation.
- Zandronum: Renamed "Flags" Create Game page to "Zandronum". The page has
more generic stuff now than just "flags" and the new name fits better.

### Fixed
- Don't reset difficulty or game limit values in "Create Game"
dialog when changing game mode.
- Auto-updater wouldn't install all files from auxiliary update packages.
This wasn't very critical for a long time as most files in auxiliary
packages (plugin's packages) weren't vital and the plugins themselves
were always installed correctly.
- Use QProcessEnvironment to obtain environment variables instead of
getenv(). The new solution should be unicode-aware.
- Downloading missing files from server list context menu:
    - Don't display "missing files" error message anymore if user chooses
      not to install optional WADs.
    - JoinCommandLineBuilder was never freed and created a memory leak.
- Possible buffer overlow in WinMain() when using characters outside
ASCII range in command line args.
- Replaced standard dialog buttons in demo manager with custom ones as
standard naming could be odd depending on platform ("close without
saving" instead of "delete"/"discard").
- Remember which PWADs were optional in recorded demos.
- "Update IP2C" menu action was available at the start of the program while
the IP2C parser/updater was still working. This behavior was incorrect,
as the menu action should always be disabled while parser/updater is working.
- Linux: Adhere to FreeDesktop standards regarding category and icon name.
- Linux: Proper "make install" destinations for:
    - Doomseeker.desktop - don't use absolute dir path, use "share/applications"
    - Doomseeker's icon.png - use "share/icons".
- Linux (openSUSE): Fix LZMA compilation issues.
- Wadseeker: Automatically abort queries to sites & services when all
files are installed.
- Wadseeker: Problems with URLs containing '%' character or non-ASCII
characters.
- Wadseeker: idgames archive now forcibly redirects HTTP to HTTPS.
Wadseeker didn't handle this properly and this functionality stopped working.
- Wadseeker: fixed ZIP extraction to gather info from central directory rather
than from local file headers, which is the correct approach for not corrupted
archives. Fixes extraction of e1m4b.wad and e1m8b.wad. Thanks, John Romero!
- Chocolate Doom: improved recognition of what IWADs are hosted on servers.
- Chocolate Doom: Strife's IWAD is strife1.wad, not strife.wad.
- Zandronum:  Fixed "Sucide" typo in one of the Create Game's flags pages.

## [1.0] - 2015-03-22
- Version 1.0 released. No changelog was kept before.

[development]: https://bitbucket.org/Doomseeker/doomseeker/commits/all
[1.1]: https://bitbucket.org/Doomseeker/doomseeker/commits/tag/1.1
[1.0]: https://bitbucket.org/Doomseeker/doomseeker/commits/tag/1.0
