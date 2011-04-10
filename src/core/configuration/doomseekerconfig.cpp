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
#include "sdeapi/pluginloader.hpp"
#include "wadseeker/wadseeker.h"
#include "ini.h"
#include "log.h"
#include "main.h"
#include "strings.h"
#include "version.h"

DoomseekerConfig* DoomseekerConfig::instance = NULL;

DoomseekerConfig::DoomseekerConfig()
{
	this->pIni = NULL;
	this->dummySection = new IniSection();
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
	
IniSection& DoomseekerConfig::iniSectionForPlugin(const QString& pluginName)
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

IniSection& DoomseekerConfig::iniSectionForPlugin(const PluginInfo* plugin)
{ 
	return iniSectionForPlugin(plugin->name);
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
	
	IniSection& sectionDoomseeker = pIni->section(doomseeker.SECTION_NAME);
	doomseeker.load(sectionDoomseeker);
	
	IniSection& sectionServerFilter = pIni->section(serverFilter.SECTION_NAME);
	serverFilter.load(sectionServerFilter);
	
	IniSection& sectionWadseeker = pIni->section(wadseeker.SECTION_NAME);
	wadseeker.load(sectionWadseeker);
	
	// Plugins should read their sections manually.
	
	return true;	
}
		
bool DoomseekerConfig::saveToFile()
{
	if (pIni == NULL)
	{
		return false;
	}
	
	const QString TOP_COMMENT = QObject::tr("This is %1 configuration file.\n\
Any modification done manually to this file is on your own risk.").arg(Version::fullVersionInfo());
	
	pIni->setIniTopComment(TOP_COMMENT);
	
	IniSection& sectionDoomseeker = pIni->section(doomseeker.SECTION_NAME);
	doomseeker.save(sectionDoomseeker);
	
	IniSection& sectionServerFilter = pIni->section(serverFilter.SECTION_NAME);
	serverFilter.save(sectionServerFilter);
	
	IniSection& sectionWadseeker = pIni->section(wadseeker.SECTION_NAME);
	wadseeker.save(sectionWadseeker);
	
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
	
	doomseeker.init(this->pIni->section(doomseeker.SECTION_NAME));
	serverFilter.init(this->pIni->section(serverFilter.SECTION_NAME));
	wadseeker.init(this->pIni->section(wadseeker.SECTION_NAME));
	
	return this->pIni->isValid();
}

//////////////////////////////////////////////////////////////////////////////
const QString DoomseekerConfig::DoomseekerCfg::SECTION_NAME = "Doomseeker";

DoomseekerConfig::DoomseekerCfg::DoomseekerCfg()
{
	this->bBotsAreNotPlayers = false;
	this->bCloseToTrayIcon = false;
	this->bHidePasswords = false;
	this->bIP2CountryAutoUpdate = true;
	this->bQueryAutoRefreshDontIfActive = true;
	this->bQueryAutoRefreshEnabled = false;
	this->bQueryOnStartup = true;	
	this->bMainWindowMaximized = false;
	this->bRecordDemo = false;
	this->bRememberConnectPassword = true;
	this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn = true;
	this->bUseTrayIcon = false;
	this->connectPassword = "";
	this->customServersColor = "#ffaa00";
	this->ip2CountryDatabaseMaximumAge = 10;
	this->ip2CountryUrl = "http://software77.net/geo-ip?DL=1";
	this->mainWindowState = "";
	this->mainWindowHeight = 0xffff;
	this->mainWindowWidth = 0xffff;
	this->mainWindowX = 0xffff;
	this->mainWindowY = 0xffff;
	this->queryAutoRefreshEverySeconds = 180;
	this->queryTimeout = 1000;
	this->queryTries = 3;
	this->previousCreateServerConfigDir = "";
	this->previousCreateServerExecDir = "";
	this->previousCreateServerWadDir = "";
	this->slotStyle = 1;
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
	section.createSetting("BotsAreNotPlayers", this->bBotsAreNotPlayers);
	section.createSetting("CloseToTrayIcon", this->bCloseToTrayIcon);
	section.createSetting("HidePasswords", this->bHidePasswords);
	section.createSetting("IP2CAutoUpdate", this->bIP2CountryAutoUpdate);
	section.createSetting("QueryAutoRefreshDontIfActive", this->bQueryAutoRefreshDontIfActive);
	section.createSetting("QueryAutoRefreshEnabled", this->bQueryAutoRefreshEnabled);
	section.createSetting("QueryOnStartup", this->bQueryOnStartup);
	section.createSetting("MainWindowMaximized", this->bMainWindowMaximized);
	section.createSetting("RecordDemo", this->bRecordDemo);
	section.createSetting("RememberConnectPassword", this->bRememberConnectPassword);
	section.createSetting("TellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn", this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn);
	section.createSetting("UseTrayIcon", this->bUseTrayIcon);
	section.createSetting("CustomServersColor", this->customServersColor);
	section.createSetting("IP2CMaximumAge", this->ip2CountryDatabaseMaximumAge);
	section.createSetting("IP2CUrl", this->ip2CountryUrl);
	section.createSetting("QueryAutoRefreshEverySeconds", this->queryAutoRefreshEverySeconds);
	section.createSetting("QueryTimeout", this->queryTimeout);
	section.createSetting("QueryTries", this->queryTries);
	section.createSetting("SlotStyle", this->slotStyle);
	section.createSetting("WadPaths", this->wadPaths.join(";"));
}

void DoomseekerConfig::DoomseekerCfg::load(IniSection& section)
{
	this->bBotsAreNotPlayers = section["BotsAreNotPlayers"];
	this->bCloseToTrayIcon = section["CloseToTrayIcon"];
	this->bHidePasswords = section["HidePasswords"];
	this->bIP2CountryAutoUpdate = section["IP2CAutoUpdate"];
	this->bQueryAutoRefreshDontIfActive = section["QueryAutoRefreshDontIfActive"];
	this->bQueryAutoRefreshEnabled = section["QueryAutoRefreshEnabled"];
	this->bQueryOnStartup = section["QueryOnStartup"];
	this->bMainWindowMaximized = section["MainWindowMaximized"];
	this->bRecordDemo = section["RecordDemo"];
	this->bRememberConnectPassword = section["RememberConnectPassword"];
	this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn = section["TellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn"];
	this->bUseTrayIcon = section["UseTrayIcon"];
	this->connectPassword = (const QString &)section["ConnectPassword"];
	this->customServersColor = (const QString &)section["CustomServersColor"];
	this->ip2CountryDatabaseMaximumAge = section["IP2CMaximumAge"];
	this->ip2CountryUrl = (const QString &)section["IP2CUrl"];
	this->mainWindowState = (const QString &)section["MainWindowState"];
	this->mainWindowHeight = section["MainWindowHeight"];
	this->mainWindowWidth = section["MainWindowWidth"];
	this->mainWindowX = section["MainWindowX"];
	this->mainWindowY = section["MainWindowY"];
	this->queryAutoRefreshEverySeconds = section["QueryAutoRefreshEverySeconds"];
	this->queryTimeout = section["QueryTimeout"];
	this->queryTries = section["QueryTries"];
	this->previousCreateServerConfigDir = (const QString &)section["PreviousCreateServerConfigDir"];
	this->previousCreateServerExecDir = (const QString &)section["PreviousCreateServerExecDir"];
	this->previousCreateServerWadDir = (const QString &)section["PreviousCreateServerWadDir"];
	this->serverListColumnState = (const QString &)section["ServerListColumnState"];
	this->slotStyle = section["SlotStyle"];
	
	// Complex data variables.
	
	// Custom servers
	QList<CustomServerInfo> customServersList;
	CustomServers::decodeConfigEntries(section["CustomServers"], customServersList);
	this->customServers = customServersList.toVector();

	// Wad paths
	QString wadPathsString = section["WadPaths"];
	wadPaths = wadPathsString.split(";", QString::SkipEmptyParts);
	
	// Buddies list
	QString buddiesConfigEntry = section["BuddiesList"];
	BuddyInfo::readConfigEntry(buddiesConfigEntry, this->buddiesList);
}

void DoomseekerConfig::DoomseekerCfg::save(IniSection& section)
{
	section["BotsAreNotPlayers"] = this->bBotsAreNotPlayers;
	section["CloseToTrayIcon"] = this->bCloseToTrayIcon;
	section["HidePasswords"] = this->bHidePasswords;
	section["IP2CAutoUpdate"] = this->bIP2CountryAutoUpdate;
	section["QueryAutoRefreshDontIfActive"] = this->bQueryAutoRefreshDontIfActive;
	section["QueryAutoRefreshEnabled"] = this->bQueryAutoRefreshEnabled;
	section["QueryOnStartup"] = this->bQueryOnStartup;
	section["MainWindowMaximized"] = this->bMainWindowMaximized;
	section["RecordDemo"] = this->bRecordDemo;
	section["RememberConnectPassword"] = this->bRememberConnectPassword;
	section["TellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn"] = this->bTellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn;
	section["UseTrayIcon"] = this->bUseTrayIcon;
	section["ConnectPassword"] = this->connectPassword;
	section["CustomServersColor"] = this->customServersColor;
	section["IP2CMaximumAge"] = this->ip2CountryDatabaseMaximumAge;
	section["IP2CUrl"] = this->ip2CountryUrl;
	section["MainWindowState"] = this->mainWindowState;
	section["MainWindowHeight"] = this->mainWindowHeight;
	section["MainWindowWidth"] = this->mainWindowWidth;
	section["MainWindowX"] = this->mainWindowX;
	section["MainWindowY"] = this->mainWindowY;
	section["QueryAutoRefreshEverySeconds"] = this->queryAutoRefreshEverySeconds;
	section["QueryTimeout"] = this->queryTimeout;
	section["QueryTries"] = this->queryTries;
	section["PreviousCreateServerConfigDir"] = this->previousCreateServerConfigDir;
	section["PreviousCreateServerExecDir"] = this->previousCreateServerExecDir;
	section["PreviousCreateServerWadDir"] = this->previousCreateServerWadDir;
	section["ServerListColumnState"] = this->serverListColumnState;
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
	QString wadPathsString = this->wadPaths.join(";");
	section["WadPaths"] = wadPathsString;
	
	// Buddies lists
	QString buddiesList = BuddyInfo::createConfigEntry(this->buddiesList);
	section["BuddiesList"] = buddiesList;
}
//////////////////////////////////////////////////////////////////////////////
const QString DoomseekerConfig::ServerFilter::SECTION_NAME = "ServerFilter";

void DoomseekerConfig::ServerFilter::init(IniSection& section)
{
	section.createSetting("bShowEmpty", true);
	section.createSetting("bShowFull", true);
	section.createSetting("bShowOnlyValid", false);
	section.createSetting("GameMode", "");
	section.createSetting("MaxPing", 0);
	section.createSetting("ServerName", "");
	section.createSetting("WADs", "");
}

void DoomseekerConfig::ServerFilter::load(IniSection& section)
{
	info.bShowEmpty = section["bShowEmpty"];
	info.bShowFull = section["bShowFull"];
	info.bShowOnlyValid = section["bShowOnlyValid"];
	info.gameMode = (const QString &)section["GameMode"];
	info.maxPing = section["MaxPing"];
	info.serverName = (const QString &)section["ServerName"];
	info.wads = ((const QString&)section["WADs"]).split(";");
}

void DoomseekerConfig::ServerFilter::save(IniSection& section)
{
	section["bShowEmpty"] = info.bShowEmpty;
	section["bShowFull"] = info.bShowFull;
	section["bShowOnlyValid"] = info.bShowOnlyValid;
	section["GameMode"] = info.gameMode;
	section["MaxPing"] = info.maxPing;
	section["ServerName"] = info.serverName;
	section["WADs"] = info.wads.join(";");
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
	this->idgamesPriority = 0;
	this->idgamesURL = Wadseeker::defaultIdgamesUrl();
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
	section.createSetting("IdgamesPriority", this->idgamesPriority);
	section.createSetting("IdgamesURL", this->idgamesURL);
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
	this->idgamesPriority = section["IdgamesPriority"];
	this->idgamesURL = (const QString &)section["IdgamesURL"];
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
	section["IdgamesPriority"] = this->idgamesPriority;
	section["IdgamesURL"] = this->idgamesURL;
	section["TargetDirectory"] = this->targetDirectory;
	
	// Complex data values
	QStringList urlEncodedList;
	foreach (const QString& url, this->searchURLs)
	{
		urlEncodedList << QUrl::toPercentEncoding(url.toAscii());
	}
	section["SearchURLs"] = urlEncodedList.join(";");
}
