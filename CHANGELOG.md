# Change Log

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
- Windows: Server list refresh progress and Wadseeker download progress
are now shown in Windows' task bar.
- Wadseeker: Queries to services (idgames & wadarchive) are shown in
the table with site queries.
- Wadseeker: Sites and services (idgames & wadarchive) queries can
be aborted by the user.
- Zandronum: LAN broadcast servers will now appear on server list.
- Zandronum: "force spectator" voting can be configured from "Create
Game" dialog.

### Changed
- Switched from Qt4 to Qt5.
- Windows: Filter files by *.exe, *.com and *.bat when browsing for
either server or client game executables.
- Windows & Mac: Update packages have been split from monolithic
'doomseeker' to smaller packages. Downloading updates won't
require to download Qt libraries everytime anymore.

### Fixed
- Don't reset difficulty in "Create Game" dialog when changing game mode.
- Wadseeker: Automatically abort queries to sites & services when all
files are installed.


## 1.0 - 2015-03-22
- Version 1.0 released. No changelog was keeped before.

[development]: https://bitbucket.org/Blzut3/doomseeker/commits/all
