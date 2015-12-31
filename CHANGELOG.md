# Change Log

Visit this file cleanly formatted in repository:
<https://bitbucket.org/Doomseeker/doomseeker/src/default/CHANGELOG.md>

This file follows recommendations in accordance to:
<http://keepachangelog.com/>

## [Development][development]
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
- Wadseeker: Added "Fetch Freedoom version info" feature to
WADSEEKER_API.
- Wadseeker: Added ModInstall class to WADSEEKER_API for mods with
known URLs.
- Wadseeker: Queries to services (idgames & wadarchive) are shown in
the table with site queries.
- Wadseeker: Sites and services (idgames & wadarchive) queries can
be aborted by the user.
- Zandronum: LAN broadcast servers will now appear on server list.
- Zandronum: "force spectator" voting can be configured from "Create
Game" dialog.
- Zandronum: display DeHackEd patches loaded on servers, allow the
game client to load them when joining.

### Changed
- Switched from Qt4 to Qt5.
- Windows: Filter files by *.exe, *.com and *.bat when browsing for
either server or client game executables.
- Windows & Mac: Update packages have been split from monolithic
'doomseeker' to smaller packages. Downloading updates won't
require to download Qt libraries everytime anymore.
- Completely reworked "missing files" dialog. Added "Install Freedoom"
button if IWAD is missing.
- To reduce noise in generated command lines, wrap with quotes only
those arguments that have characters outside of a very narrow, safe
range.

### Fixed
- Don't reset difficulty in "Create Game" dialog when changing game mode.
- Auto-updater wouldn't install all files from auxiliary update packages.
This wasn't very critical for a long time as most files in auxiliary
packages (plugin's packages) weren't vital and the plugins themselves
were always installed correctly.
- Use QProcessEnvironment to obtain environment variables instead of
getenv(). The new solution should be unicode-aware.
- Downloading missing files from server list context menu:
    - Don't display "missing files" error message anymore if user chooses
      not to install optional WADs.
    - Treat all files as optional and allow user to select them.
    - JoinCommandLineBuilder was never freed and created a memory leak.
- Possible buffer overlow in WinMain() when using characters outside
ASCII range in command line args.
- Replaced standard dialog buttons in demo manager with custom ones as
standard naming could be odd depending on platform ("close without
saving" instead of "delete"/"discard").
- Remember which PWADs were optional in recorded demos.
- Wadseeker: Automatically abort queries to sites & services when all
files are installed.
- Wadseeker: Problems with URLs containing '%' character or non-ASCII
characters.
- Chocolate Doom: improved recognition of what IWADs are hosted on servers.

## [1.0] - 2015-03-22
- Version 1.0 released. No changelog was keeped before.

[development]: https://bitbucket.org/Doomseeker/doomseeker/commits/all
[1.0]: https://bitbucket.org/Doomseeker/doomseeker/commits/tag/1.0
