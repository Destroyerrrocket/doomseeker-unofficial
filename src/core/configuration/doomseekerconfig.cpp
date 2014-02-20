//------------------------------------------------------------------------------
// doomseekerconfig.cpp
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "doomseekerconfig.h"
#include "ini/ini.h"
#include "ini/inisection.h"
#include "ini/inivariable.h"
#include "pathfinder/filesearchpath.h"
#include "plugins/engineplugin.h"
#include "updater/updatechannel.h"
#include "wadseeker/wadseeker.h"
#include "log.h"
#include "main.h"
#include "strings.h"
#include "version.h"

#include <QLocale>

DoomseekerConfig* DoomseekerConfig::instance = NULL;

DoomseekerConfig::DoomseekerConfig()
{
	this->pIni = NULL;
	this->dummySection = new IniSection(NULL, QString());
}

DoomseekerConfig::~DoomseekerConfig()
{
	if (this->pIni != NULL)
	{
		delete this->pIni;
	}

	delete this->dummySection;
}

DoomseekerConfig& DoomseekerConfig::config()
{
	if (instance == NULL)
	{
		instance = new DoomseekerConfig();
	}

	return *instance;
}

void DoomseekerConfig::dispose()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}

IniSection DoomseekerConfig::iniSectionForPlugin(const QString& pluginName)
{
	if (pluginName.isEmpty())
	{
		gLog << QObject::tr("DoomseekerConfig.iniSectionForPlugin(): empty plugin name has been specified, returning dummy IniSection.");
		return *dummySection;
	}

	if (!isValidPluginName(pluginName))
	{
		gLog << QObject::tr("DoomseekerConfig.iniSectionForPlugin(): plugin name is invalid: %1").arg(pluginName);
		return *dummySection;
	}

	if (this->pIni == NULL)
	{
		setIniFile("");
	}

	QString sectionName = pluginName;
	sectionName = sectionName.replace(' ', "");
	return this->pIni->createSection(sectionName);
}

IniSection DoomseekerConfig::iniSectionForPlugin(const EnginePlugin* plugin)
{
	return iniSectionForPlugin(plugin->data()->name);
}

bool DoomseekerConfig::isValidPluginName(const QString& pluginName) const
{
	QString invalids[] = { "doomseeker", "wadseeker", "" };

	for (int i = 0; !invalids[i].isEmpty(); ++i)
	{
		if (pluginName.compare(invalids[i], Qt::CaseInsensitive) == 0)
		{
			return false;
		}
	}

	return true;
}

bool DoomseekerConfig::readFromFile()
{
	if (pIni == NULL)
	{
		return false;
	}

	IniSection sectionDoomseeker = pIni->section(doomseeker.SECTION_NAME);
	doomseeker.load(sectionDoomseeker);

	IniSection sectionServerFilter = pIni->section(serverFilter.SECTION_NAME);
	serverFilter.load(sectionServerFilter);

	IniSection sectionWadseeker = pIni->section(wadseeker.SECTION_NAME);
	wadseeker.load(sectionWadseeker);

	IniSection sectionAutoUpdates = pIni->section(autoUpdates.SECTION_NAME);
	autoUpdates.load(sectionAutoUpdates);

	// Plugins should read their sections manually.

	return true;
}

bool DoomseekerConfig::saveToFile()
{
	if (pIni == NULL)
	{
		return false;
	}

// TODO:
// Find a way to work around this.
//	const QString TOP_COMMENT = QObject::tr("This is %1 configuration file.\n\
//Any modification done manually to this file is on your own risk.").arg(Version::fullVersionInfo());
//
//	pIni->setIniTopComment(TOP_COMMENT);

	IniSection sectionDoomseeker = pIni->section(doomseeker.SECTION_NAME);
	doomseeker.save(sectionDoomseeker);

	IniSection sectionServerFilter = pIni->section(serverFilter.SECTION_NAME);
	serverFilter.save(sectionServerFilter);

	IniSection sectionWadseeker = pIni->section(wadseeker.SECTION_NAME);
	wadseeker.save(sectionWadseeker);

	IniSection sectionAutoUpdates = pIni->section(autoUpdates.SECTION_NAME);
	autoUpdates.save(sectionAutoUpdates);

	// Plugins should save their sections manually.

	return pIni->save();
}

bool DoomseekerConfig::setIniFile(const QString& filePath)
{
	if (this->pIni != NULL)
	{
		delete this->pIni;
	}

	gLog << QObject::tr("Setting INI file: %1").arg(filePath);
	this->pIni = new Ini(filePath);

	IniSection section;

	section = this->pIni->section(doomseeker.SECTION_NAME);
	doomseeker.init(section);

	section = this->pIni->section(serverFilter.SECTION_NAME);
	serverFilter.init(section);

	section = this->pIni->section(wadseeker.SECTION_NAME);
	wadseeker.init(section);

	section = this->pIni->section(autoUpdates.SECTION_NAME);
	autoUpdates.init(section);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
const QString DoomseekerConfig::DoomseekerCfg::SECTION_NAME = "Doomseeker";

DoomseekerConfig::DoomseekerCfg::DoomseekerCfg()
{
	this->bBotsAreNotPlayers = true;
	this->bCloseToTrayIcon = false;
	this->bColorizeServerConsole = true;
	this->bDrawGridInServerTable = false;
	this->bHidePasswords = false;
	this->bIP2CountryAutoUpdate = true;
	this->bLookupHosts = true;
	this->bQueryAutoRefreshDontIfActive = true;
	this->bQueryAutoRefreshEnabled = false;
	this->bQueryBeforeLaunch = true;
	this->bQueryOnStartup = true;
	this->bMainWindowMaximized = false;
	this->bRecordDemo = false;
	this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn = true;
	this->bUseTrayIcon = false;
	this->customServersColor = "#ffaa00";
	this->ip2CountryDatabaseMaximumAge = 60;
	this->ip2CountryUrl = "http://doomseeker.drdteam.org/ip2c/get";
	this->localization = QLocale::system().name();
	this->mainWindowState = "";
	this->mainWindowHeight = 0xffff;
	this->mainWindowWidth = 0xffff;
	this->mainWindowX = 0xffff;
	this->mainWindowY = 0xffff;
	this->queryAutoRefreshEverySeconds = 180;
	this->queryBatchSize = 30;
	this->queryBatchDelay = 50;
	this->queryTimeout = 1000;
	this->queryTries = 3;
	this->previousCreateServerConfigDir = "";
	this->previousCreateServerExecDir = "";
	this->previousCreateServerWadDir = "";
	this->slotStyle = 1;
	this->serverListSortIndex = -1;
	this->serverListSortDirection = Qt::DescendingOrder;
	this->wadPaths << Main::dataPaths->programsDataDirectoryPath();
	this->wadPaths << Main::workingDirectory;
}

bool DoomseekerConfig::DoomseekerCfg::areMainWindowSizeSettingsValid(int maxValidX, int maxValidY) const
{
	int endX = this->mainWindowX + this->mainWindowWidth;
	int endY = this->mainWindowY + this->mainWindowHeight;

	if (endX <= 0 || endY <= 0)
	{
		return false;
	}

	if (this->mainWindowX > maxValidX
	||  this->mainWindowY > maxValidY)
	{
		return false;
	}

	if (this->mainWindowX < -2 * maxValidX
	||  this->mainWindowY < -2 * maxValidY)
	{
		return false;
	}

	if (this->mainWindowWidth > 2 * (unsigned)maxValidX
	||  this->mainWindowWidth == 0)
	{
		return false;
	}

	if (this->mainWindowHeight > 2 * (unsigned)maxValidY
	|| this->mainWindowHeight == 0)
	{
		return false;
	}

	return true;
}

void DoomseekerConfig::DoomseekerCfg::init(IniSection& section)
{
	section.createSetting("Localization", this->localization);
	section.createSetting("BotsAreNotPlayers", this->bBotsAreNotPlayers);
	section.createSetting("CloseToTrayIcon", this->bCloseToTrayIcon);
	section.createSetting("ColorizeServerConsole", this->bColorizeServerConsole);
	section.createSetting("DrawGridInServerTable", this->bDrawGridInServerTable);
	section.createSetting("HidePasswords", this->bHidePasswords);
	section.createSetting("IP2CAutoUpdate", this->bIP2CountryAutoUpdate);
	section.createSetting("LookupHosts", this->bLookupHosts);
	section.createSetting("QueryAutoRefreshDontIfActive", this->bQueryAutoRefreshDontIfActive);
	section.createSetting("QueryAutoRefreshEnabled", this->bQueryAutoRefreshEnabled);
	section.createSetting("QueryOnStartup", this->bQueryOnStartup);
	section.createSetting("MainWindowMaximized", this->bMainWindowMaximized);
	section.createSetting("RecordDemo", this->bRecordDemo);
	section.createSetting("TellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn", this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn);
	section.createSetting("UseTrayIcon", this->bUseTrayIcon);
	section.createSetting("CustomServersColor", this->customServersColor);
	section.createSetting("IP2CMaximumAge", this->ip2CountryDatabaseMaximumAge);
	section.createSetting("IP2CUrl", this->ip2CountryUrl);
	section.createSetting("QueryAutoRefreshEverySeconds", this->queryAutoRefreshEverySeconds);
	section.createSetting("QueryBatchSize", this->queryBatchSize);
	section.createSetting("QueryBatchDelay", this->queryBatchDelay);
	section.createSetting("QueryTimeout", this->queryTimeout);
	section.createSetting("QueryTries", this->queryTries);
	section.createSetting("SlotStyle", this->slotStyle);
	section.createSetting("ServerListSortIndex", this->serverListSortIndex);
	section.createSetting("ServerListSortDirection", this->serverListSortDirection);
	section.createSetting("WadPaths", FileSearchPath::toVariantList(this->wadPaths));
}

void DoomseekerConfig::DoomseekerCfg::load(IniSection& section)
{
	this->localization = (const QString&)section["Localization"];
	this->bBotsAreNotPlayers = section["BotsAreNotPlayers"];
	this->bCloseToTrayIcon = section["CloseToTrayIcon"];
	this->bColorizeServerConsole = section["ColorizeServerConsole"];
	this->bDrawGridInServerTable = section["DrawGridInServerTable"];
	this->bHidePasswords = section["HidePasswords"];
	this->bIP2CountryAutoUpdate = section["IP2CAutoUpdate"];
	this->bLookupHosts = section["LookupHosts"];
	this->bQueryAutoRefreshDontIfActive = section["QueryAutoRefreshDontIfActive"];
	this->bQueryAutoRefreshEnabled = section["QueryAutoRefreshEnabled"];
	this->bQueryBeforeLaunch = section["QueryBeforeLaunch"];
	this->bQueryOnStartup = section["QueryOnStartup"];
	this->bMainWindowMaximized = section["MainWindowMaximized"];
	this->bRecordDemo = section["RecordDemo"];
	this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn = section["TellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn"];
	this->bUseTrayIcon = section["UseTrayIcon"];
	this->customServersColor = (const QString &)section["CustomServersColor"];
	this->ip2CountryDatabaseMaximumAge = section["IP2CMaximumAge"];
	this->ip2CountryUrl = (const QString &)section["IP2CUrl"];
	this->mainWindowState = (const QString &)section["MainWindowState"];
	this->mainWindowHeight = section["MainWindowHeight"];
	this->mainWindowWidth = section["MainWindowWidth"];
	this->mainWindowX = section["MainWindowX"];
	this->mainWindowY = section["MainWindowY"];
	this->queryAutoRefreshEverySeconds = section["QueryAutoRefreshEverySeconds"];
	this->queryBatchSize = section["QueryBatchSize"];
	this->queryBatchDelay = section["QueryBatchDelay"];
	this->queryTimeout = section["QueryTimeout"];
	this->queryTries = section["QueryTries"];
	this->previousCreateServerConfigDir = (const QString &)section["PreviousCreateServerConfigDir"];
	this->previousCreateServerExecDir = (const QString &)section["PreviousCreateServerExecDir"];
	this->previousCreateServerWadDir = (const QString &)section["PreviousCreateServerWadDir"];
	this->serverListColumnState = (const QString &)section["ServerListColumnState"];
	this->serverListSortIndex = section["ServerListSortIndex"];
	this->serverListSortDirection = section["ServerListSortDirection"];
	this->slotStyle = section["SlotStyle"];

	// Complex data variables.

	// Custom servers
	QList<CustomServerInfo> customServersList;
	CustomServers::decodeConfigEntries(section["CustomServers"], customServersList);
	this->customServers = customServersList.toVector();

	// WAD paths
	// Backward compatibility, XXX once new stable is released:
	QVariant variantWadPaths = section["WadPaths"].value();
	if (variantWadPaths.isValid() && variantWadPaths.toList().isEmpty())
	{
		// Backward compatibility continued:
		wadPaths.clear();
		QStringList paths = variantWadPaths.toString().split(";");
		foreach (const QString& path, paths)
		{
			wadPaths << FileSearchPath(path);
		}
	}
	else
	{
		// This is not a part of XXX, this is proper, current behavior:
		wadPaths = FileSearchPath::fromVariantList(section["WadPaths"].value().toList());
	}
	// End of backward compatibility for WAD paths.

	// Buddies list
	QString buddiesConfigEntry = section["BuddiesList"];
	BuddyInfo::readConfigEntry(buddiesConfigEntry, this->buddiesList);
}

void DoomseekerConfig::DoomseekerCfg::save(IniSection& section)
{
	section["Localization"] = this->localization;
	section["BotsAreNotPlayers"] = this->bBotsAreNotPlayers;
	section["CloseToTrayIcon"] = this->bCloseToTrayIcon;
	section["ColorizeServerConsole"] = this->bColorizeServerConsole;
	section["DrawGridInServerTable"] = this->bDrawGridInServerTable;
	section["HidePasswords"] = this->bHidePasswords;
	section["IP2CAutoUpdate"] = this->bIP2CountryAutoUpdate;
	section["LookupHosts"] = this->bLookupHosts;
	section["QueryAutoRefreshDontIfActive"] = this->bQueryAutoRefreshDontIfActive;
	section["QueryAutoRefreshEnabled"] = this->bQueryAutoRefreshEnabled;
	section["QueryBeforeLaunch"] = this->bQueryBeforeLaunch;
	section["QueryOnStartup"] = this->bQueryOnStartup;
	section["MainWindowMaximized"] = this->bMainWindowMaximized;
	section["RecordDemo"] = this->bRecordDemo;
	section["TellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn"] = this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn;
	section["UseTrayIcon"] = this->bUseTrayIcon;
	section["CustomServersColor"] = this->customServersColor;
	section["IP2CMaximumAge"] = this->ip2CountryDatabaseMaximumAge;
	section["IP2CUrl"] = this->ip2CountryUrl;
	section["MainWindowState"] = this->mainWindowState;
	section["MainWindowHeight"] = this->mainWindowHeight;
	section["MainWindowWidth"] = this->mainWindowWidth;
	section["MainWindowX"] = this->mainWindowX;
	section["MainWindowY"] = this->mainWindowY;
	section["QueryAutoRefreshEverySeconds"] = this->queryAutoRefreshEverySeconds;
	section["QueryBatchSize"] = this->queryBatchSize;
	section["QueryBatchDelay"] = this->queryBatchDelay;
	section["QueryTimeout"] = this->queryTimeout;
	section["QueryTries"] = this->queryTries;
	section["PreviousCreateServerConfigDir"] = this->previousCreateServerConfigDir;
	section["PreviousCreateServerExecDir"] = this->previousCreateServerExecDir;
	section["PreviousCreateServerWadDir"] = this->previousCreateServerWadDir;
	section["ServerListColumnState"] = this->serverListColumnState;
	section["ServerListSortIndex"] = this->serverListSortIndex;
	section["ServerListSortDirection"] = this->serverListSortDirection;
	section["SlotStyle"] = this->slotStyle;

	// Complex data variables.

	// Custom servers
	QStringList allCustomServers;
	foreach (const CustomServerInfo& customServer, this->customServers)
	{
		QString engineName = QUrl::toPercentEncoding(customServer.engine, "", "()");
		QString address = QUrl::toPercentEncoding(customServer.host, "", "()");

		QString customServerString = QString("(%1;%2;%3)")
			.arg(engineName).arg(address)
			.arg(customServer.port);

		allCustomServers << customServerString;
	}
	section["CustomServers"] = allCustomServers.join(";");

	// Wad paths
	section["WadPaths"].setValue(FileSearchPath::toVariantList(this->wadPaths));

	// Buddies lists
	QString buddiesList = BuddyInfo::createConfigEntry(this->buddiesList);
	section["BuddiesList"] = buddiesList;
}

QStringList DoomseekerConfig::DoomseekerCfg::wadPathsOnly() const
{
	QStringList result;
	foreach (const FileSearchPath& path, wadPaths)
	{
		result << path.path();
	}
	return result;
}
//////////////////////////////////////////////////////////////////////////////
const QString DoomseekerConfig::AutoUpdates::SECTION_NAME = "Doomseeker_AutoUpdates";

void DoomseekerConfig::AutoUpdates::init(IniSection& section)
{
	section.createSetting("UpdateChannelName", UpdateChannel::mkStable().name());
	section.createSetting("UpdateMode", (int) UM_NotifyOnly);
	section.createSetting("LastKnownUpdateRevisions", QVariant());
	section.createSetting("bPerformUpdateOnNextRun", false);
}

void DoomseekerConfig::AutoUpdates::load(IniSection& section)
{
	updateChannelName = (const QString &)section["UpdateChannelName"];
	updateMode = (UpdateMode)section["UpdateMode"].value().toInt();
	QVariantMap lastKnownUpdateRevisionsVariant = section["LastKnownUpdateRevisions"].value().toMap();
	lastKnownUpdateRevisions.clear();
	foreach (const QString& package, lastKnownUpdateRevisionsVariant.keys())
	{
		QVariant revisionVariant = lastKnownUpdateRevisionsVariant[package];
		lastKnownUpdateRevisions.insert(package, revisionVariant.toLongLong());
	}
	bPerformUpdateOnNextRun = section["bPerformUpdateOnNextRun"].value().toBool();
}

void DoomseekerConfig::AutoUpdates::save(IniSection& section)
{
	section["UpdateChannelName"] = updateChannelName;
	section["UpdateMode"] = updateMode;
	QVariantMap revisionsVariantMap;
	foreach (const QString& package, lastKnownUpdateRevisions.keys())
	{
		revisionsVariantMap.insert(package, lastKnownUpdateRevisions[package]);
	}
	section["LastKnownUpdateRevisions"].setValue(revisionsVariantMap);
	section["bPerformUpdateOnNextRun"].setValue(bPerformUpdateOnNextRun);
}
//////////////////////////////////////////////////////////////////////////////
const QString DoomseekerConfig::ServerFilter::SECTION_NAME = "ServerFilter";

void DoomseekerConfig::ServerFilter::init(IniSection& section)
{
	section.createSetting("bShowEmpty", true);
	section.createSetting("bShowFull", true);
	section.createSetting("bShowOnlyValid", false);
	section.createSetting("GameModes", QStringList());
	section.createSetting("GameModesExcluded", QStringList());
	section.createSetting("MaxPing", 0);
	section.createSetting("ServerName", "");
	section.createSetting("WADs", QStringList());
	section.createSetting("WADsExcluded", QStringList());
}

void DoomseekerConfig::ServerFilter::load(IniSection& section)
{
	info.bShowEmpty = section["bShowEmpty"];
	info.bShowFull = section["bShowFull"];
	info.bShowOnlyValid = section["bShowOnlyValid"];
	info.gameModes = section["GameModes"].value().toStringList();
	info.gameModesExcluded = section["GameModesExcluded"].value().toStringList();
	info.maxPing = section["MaxPing"];
	info.serverName = (const QString &)section["ServerName"];
	info.wads = section["WADs"].value().toStringList();
	info.wadsExcluded = section["WADsExcluded"].value().toStringList();
}

void DoomseekerConfig::ServerFilter::save(IniSection& section)
{
	section["bShowEmpty"] = info.bShowEmpty;
	section["bShowFull"] = info.bShowFull;
	section["bShowOnlyValid"] = info.bShowOnlyValid;
	section["GameModes"].setValue(info.gameModes);
	section["GameModesExcluded"].setValue(info.gameModesExcluded);
	section["MaxPing"] = info.maxPing;
	section["ServerName"] = info.serverName;
	section["WADs"].setValue(info.wads);
	section["WADsExcluded"].setValue(info.wadsExcluded);
}
//////////////////////////////////////////////////////////////////////////////
const QString DoomseekerConfig::WadseekerCfg::SECTION_NAME = "Wadseeker";

DoomseekerConfig::WadseekerCfg::WadseekerCfg()
{
	this->bSearchInIdgames = true;
	this->colorMessageCriticalError = "#ff0000";
	this->colorMessageError = "#ff0000";
	this->colorMessageNotice = "#000000";
	this->connectTimeoutSeconds = WADSEEKER_CONNECT_TIMEOUT_SECONDS_DEFAULT;
	this->downloadTimeoutSeconds = WADSEEKER_DOWNLOAD_TIMEOUT_SECONDS_DEFAULT;
	this->idgamesURL = Wadseeker::defaultIdgamesUrl();
	this->maxConcurrentSiteDownloads = 3;
	this->maxConcurrentWadDownloads = 2;
	this->targetDirectory = Main::dataPaths->programsDataDirectoryPath();

	// Search URLs remains unitizalized here. It will be initialized
	// by init() and then load() since Doomseeker always calls these
	// methods in this order.
}

void DoomseekerConfig::WadseekerCfg::init(IniSection& section)
{
	section.createSetting("SearchInIdgames", this->bSearchInIdgames);
	section.createSetting("ColorMessageCriticalError", this->colorMessageCriticalError);
	section.createSetting("ColorMessageError", this->colorMessageError);
	section.createSetting("ColorMessageNotice", this->colorMessageNotice);
	section.createSetting("ConnectTimeoutSeconds", this->connectTimeoutSeconds);
	section.createSetting("DownloadTimeoutSeconds", this->downloadTimeoutSeconds);
	section.createSetting("IdgamesURL", this->idgamesURL);
	section.createSetting("MaxConcurrentSiteDownloads", this->maxConcurrentSiteDownloads);
	section.createSetting("MaxConcurrentWadDownloads", this->maxConcurrentWadDownloads);
	section.createSetting("SearchURLs", Wadseeker::defaultSitesListEncoded().join(";"));
	section.createSetting("TargetDirectory", this->targetDirectory);
}

void DoomseekerConfig::WadseekerCfg::load(IniSection& section)
{
	this->bSearchInIdgames = section["SearchInIdgames"];
	this->colorMessageCriticalError = (const QString &)section["ColorMessageCriticalError"];
	this->colorMessageError = (const QString &)section["ColorMessageError"];
	this->colorMessageNotice = (const QString &)section["ColorMessageNotice"];
	this->connectTimeoutSeconds = section["ConnectTimeoutSeconds"];
	this->downloadTimeoutSeconds = section["DownloadTimeoutSeconds"];
	this->idgamesURL = (const QString &)section["IdgamesURL"];
	this->maxConcurrentSiteDownloads = section["MaxConcurrentSiteDownloads"];
	this->maxConcurrentWadDownloads = section["MaxConcurrentWadDownloads"];
	this->targetDirectory = (const QString &)section["TargetDirectory"];

	// Complex data values
	this->searchURLs.clear();
	QStringList urlList = section["SearchURLs"].valueString().split(";", QString::SkipEmptyParts);
	foreach (const QString& url, urlList)
	{
		this->searchURLs << QUrl::fromPercentEncoding(url.toAscii());
	}
}

void DoomseekerConfig::WadseekerCfg::save(IniSection& section)
{
	section["SearchInIdgames"] = this->bSearchInIdgames;
	section["ColorMessageCriticalError"] = this->colorMessageCriticalError;
	section["ColorMessageError"] = this->colorMessageError;
	section["ColorMessageNotice"] = this->colorMessageNotice;
	section["ConnectTimeoutSeconds"] = this->connectTimeoutSeconds;
	section["DownloadTimeoutSeconds"] = this->downloadTimeoutSeconds;
	section["IdgamesURL"] = this->idgamesURL;
	section["MaxConcurrentSiteDownloads"] = this->maxConcurrentSiteDownloads;
	section["MaxConcurrentWadDownloads"] = this->maxConcurrentWadDownloads;
	section["TargetDirectory"] = this->targetDirectory;

	// Complex data values
	QStringList urlEncodedList;
	foreach (const QString& url, this->searchURLs)
	{
		urlEncodedList << QUrl::toPercentEncoding(url.toAscii());
	}
	section["SearchURLs"] = urlEncodedList.join(";");
}

