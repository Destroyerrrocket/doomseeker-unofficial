# Change Log

Visit this file cleanly formatted in repository:
<https://bitbucket.org/Doomseeker/doomseeker/src/default/CHANGELOG.md>

This file follows recommendations in accordance to:
<http://keepachangelog.com/>

## [Development][development]
### Added
- List the '--help' argument in the help.
- '--version-json' argument can now write to stdout if file is not specified
or specified as '-'.
- "Remove IRC network" button was added to the IRC network selection dialog.
- Attributions for icons added to the about dialog.
- Allow to load player slots styles from "theme/slots" directories that can
be placed in the static search paths.
- Extra CA certificates for SSL will be loaded from "cacerts.pem" file
if such file is present in one of the program's static data directories
(so, right next to the .exe on Windows).
- It can now be controlled whether logs are printed to stderr
by '--quiet' and '--verbose' command line switches. Logging
to the "Logs" box will happen regardless of this switch.
- RCon connection box can now toggle showing/hiding passwords.
- Create Game misc. tab can now toggle showing/hiding passwords.
- Button to show JSON library license added to the About dialog (addresses 3451).
- Servers can now be dynamically pinned or unpinned using the server
list's context menu. This adds them to the "Custom/Pinned Servers"
list and essentially servers the purpose of marking servers
as favourite (addresses 3388).
- "Custom/Pinned" servers can be disabled without removing
them from configuration.
- File aliases can now be either configured as "left-to-right" where a single
file (WAD) can be replaced by one of aliases, or can be marked as equal where
each file is replaceable by any other file on the same list. (addresses 2740).
- '--create-game' argument added; it launches Doomseeker into the
"Create Game" box, omitting the main window altogether. (addresses 3479)
- Allow to pick *no* game mode and *no* difficulty level when creating
a game. The idea here is to allow the game to decide on these values or
to start the game into the title screen instead of dropping immediately
into the map. (addresses 3505)
- IRC: User name can now be specified (this is different than nick name).
- Windows: cacerts.pem database with "DST Root CA X3" and
"DigiCert High Assurance EV Root CA" certificates.
- Zandronum: Support "sv_deadplayerscankeepinventory" zadmflag.

### Changed
- **Relicensed Doomseeker and plugins to LGPLv2.1+.**
- Reworked data paths so Doomseeker should now follow XDG on Linux (Qt5 only).
Configs will be migrated. On Mac the demos are migrated to Application Support.
On Windows, paths will be moved to %LOCALAPPDATA% and leading dot is removed
from "doomseeker" directory name.
Otherwise paths are more or less the same as before.
- IRC network selection dialog buttons have only the icons and no text now.
- Replaced any hardcoded HTTP addresses with HTTPS wherever applicable & possible.
- Replaced several icons that would violate the LGPLv2.1+ licensing.
Most notably, the "Buddies" icon that depicted 2 Doomguys was replaced
with 2 ballheads. Sorry, I hate this too.
- "Marines" slot style is considered non-free and conflicting with Doomseeker's
licensing, hence it will now be distributed as slot theme using the new theming
system. This change should be transparent to the end-user.
- To support slot themes, changed how slot style is saved in the configuration.
The setting will not be preserved correctly when going back to prior versions
of Doomseeker.
- If Doomseeker is installed into a directory named "bin" (typical on Linux)
or "MacOS" (application bundle), the binary directory will no longer be added to
the wad file paths by default.
- Updated the internal zlib library to 1.2.11. This affects Windows builds.
- Logs will now be printed to stderr instead of stdout.
- Only show games that support Remote Console in RCon connection box.
- Starting Doomseeker with incorrect command line arguments will now
print help and quit immediately (addresses 3294).
- Reword a bit the log and error messages associated with total server
refresh - don't say that "the operation is senseless" and don't notify
about problem with deselected masters when there are some permanent
(pinned) servers on the list.
- "Custom Servers" are now known as "Pinned Servers" (addresses 3388).
- Custom Servers config box: "Set Engine" button now says "Set Game".
- When server is already "pinned", prevent it from appearing twice
on the list if master server also reports it.
- Several appearance settings, such as server table colors, player slots
style and grid will now be dynamically applied as the user changes them
in the configuration box. This allows for quick preview (addresses 3306).
- Linux: Also try to look for WADs in /usr/[local]/share/games/doom.
- Linux: Explicit "Copy (text)" actions shall store the data both in keyboard
and mouse selection clipboards, allowing it to be pasted with either keyboard
or middle mouse button (addresses 3257).
- Linux: Move plugins to /usr/lib/doomseeker (addresses 3232).
- Windows: install with the "INSTALL" target the license file for the updater.
- Wadseeker, API: Removed FixedNetworkAccessManager class from API.
- Odamex: When launching game client, use Doomseeker's WAD finding mechanism
specialized for this task instead of the generic one.
- Zandronum: Zandronum 3 was released, so made its ruleset the default in
Create Game box and removed its beta status.
- Zandronum: default testing binaries path is now in "local data"
directory, not in "documents".

### Fixed
- Count players and decrease refreshing servers amount just once for
each server. This should fix incorrect numbers in the status bar.
- Display Doomseeker icon in Demo Manager dialog.
- Set correct title on IRC network selection dialog.
- Adjusted size of the configuration dialog and its widgets to minimize
the risk of its size jumping around when browsing or the text becoming
hidden behind ellipsis.
- Fix broken English "time has past" (addresses 3269).
- "Refresh server before game launch" option did not work at all.
- Manually refreshing a server for which a Wadseeker window was open would
show "Wadseeker is running, do you wish to ignore missing files" window.
If user clicked "cancel" or "ignore" on such dialog box, Doomseeker crashed
(addresses 3268).
- Prevent infinite IP2CLoader loop if IP2C fallback is broken. This could
never happen in reality because an always valid IP2C fallback is always baked
into the executable.
- It was possible to create an IRC network configuration with no address
or port 0, but it was impossible to remove it.
- The widget that displayed IRC network port when connecting to IRC network
had a minimum value set to 6667. If IRC network used a port lower than 6667,
it would not display properly.
- Remote Console connect dialog had no title.
- Starting Doomseeker with "--rcon" argument and then immediately closing
the connection prompt or failing it would get Doomseeker stuck running forever.
- Connection prompt box with "--rcon" argument would not be centered
on the screen because its parent widget was not yet drawn when
the prompt was first shown.
- Players with TEXTCOLO color names (custom colors in brackets) would not
properly show up on the buddy list (addresses 3193).
- Player score would be displayed incorrectly if it exceeded 32768.
Moreover, it was also wrongly assumed that the player score cannot be negative.
- Show more precise error messages when there are issues with one of the
Doomseeker's data directories other than "failure to create" (addresses 3396).
- Fixed problem with exposing hidden columns or other unintended behavior in
servers table when restoring state from a saved config. (addresses 3411)
- Ensure contents of copy text dialog are displayed from the top.
- Server counting widgets would not mark servers that come in with
already known data as "counted", which resulted in counting them twice
upon removal, which throwed the refresh counter off. This problem was
triggered by "Custom/Pinned Servers".
- Be more flexible when matching locale names to program localizations
definitions. Try to match locales like "en_GB" to the default English
translation. Try to match known localization definition just basing on
the "language" part of the "language_country" locale code. This should
fix issues when loading translations for a matching language but
mismatching country and with "Appearance" config box displaying an
invalid translation for a language that is known to the
program. (addresses #3260)
- As English translation is hardcoded into the program, it's no longer
necessary to explicitly mention it in the .def file. It is now
available by default regardless if there are any .def files or if any
of those files contain it. This change should be backward-compatible
with .def files that define localization for the invalid 'en_EN' locale
- Try to remember previously selected game modifier when the "Rules" panel
in Create Game box resets upon actions such as game or game mode change.
- IRC: connect to networks by using the hostnames directly, allowing
Qt to choose the most appropriate address. This should prefer IPv6 connectivity
when available.
- Linux: The name of the desktop file is now a reverse Doomseeker domain.
- Zandronum: fix problem with widgets for a different Zandronum version being
visible when first opening the "Create Game" box.
- Zandronum: fix tabulation focus ordering for Zandronum's Create Server page.

## [1.1-p1] - 2017-07-17
### Added
- Turok 2 EX plugin.

### Fixed
- Disconnect QProcess's signals from "Standard Server Console" when
closing. This should prevent "process closed with SIGKILL (exit
code 9)" message that could appear whenever Doomseeker popped
up a server console window.
- "Standard Server Console" will also try to gracefully terminating
the process first and only issue a SIGKILL after a 1 second timeout.
- Zandronum: use "$@" instead of $* in Linux launch scripts for
testing builds. (also add a trailing \n to these scripts)
- Zandronum: dmflags values from some widgets weren't applied when
loading server config.

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


Copyright (this file)
---------------------
Copyright (C) 2015 The Doomseeker Team

Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.  This file is offered as-is,
without any warranty.


[development]: https://bitbucket.org/Doomseeker/doomseeker/commits/all
[1.1-p1]: https://bitbucket.org/Doomseeker/doomseeker/commits/tag/1.1-p1
[1.1]: https://bitbucket.org/Doomseeker/doomseeker/commits/tag/1.1
[1.0]: https://bitbucket.org/Doomseeker/doomseeker/commits/tag/1.0
