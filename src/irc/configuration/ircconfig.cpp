//------------------------------------------------------------------------------
// ircconfig.cpp
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
#include "ircconfig.h"
#include "log.h"
#include "main.h"
#include "version.h"

IRCConfig* IRCConfig::instance = NULL;

IRCConfig::IRCConfig()
{
	this->pIni = NULL;
}

IRCConfig::~IRCConfig()
{
	if (this->pIni != NULL)
	{
		delete this->pIni;
	}
}

IRCConfig& IRCConfig::config()
{
	if (instance == NULL)
	{
		instance = new IRCConfig();
	}
	
	return *instance;
}

void IRCConfig::dispose()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}
	
bool IRCConfig::readFromFile()
{
	if (pIni == NULL)
	{
		return false;
	}
	
	IniSection* pSection;
	
	pSection = &pIni->section(AppearanceCfg::SECTION_NAME);
	appearance.load(*pSection);
	
	pSection = &pIni->section(GeneralCfg::SECTION_NAME);
	general.load(*pSection);
	
	pSection = &pIni->section(PersonalCfg::SECTION_NAME);
	personal.load(*pSection);
	
	networks.load(*pIni);
	
	return true;	
}
		
bool IRCConfig::saveToFile()
{
	if (pIni == NULL)
	{
		return false;
	}
	
	const QString TOP_COMMENT = QObject::tr("This is %1 IRC module configuration file.\n\
Any modification done manually to this file is on your own risk.").arg(Version::fullVersionInfo());
	
	pIni->setIniTopComment(TOP_COMMENT);	
	
	IniSection* pSection;
	
	pSection = &pIni->section(AppearanceCfg::SECTION_NAME);
	appearance.save(*pSection);
	
	pSection = &pIni->section(GeneralCfg::SECTION_NAME);
	general.save(*pSection);
	
	pSection = &pIni->section(PersonalCfg::SECTION_NAME);
	personal.save(*pSection);
	
	networks.save(*pIni);
	
	return pIni->save();
}

bool IRCConfig::setIniFile(const QString& filePath)
{
	if (this->pIni != NULL)
	{
		delete this->pIni;
	}
	
	gLog << QObject::tr("Setting IRC INI file: %1").arg(filePath);
	this->pIni = new Ini(filePath);
	
	appearance.init(this->pIni->section(AppearanceCfg::SECTION_NAME));
	
	return this->pIni->isValid();
}

//////////////////////////////////////////////////////////////////////////////
const QString IRCConfig::AppearanceCfg::SECTION_NAME = "Appearance";

IRCConfig::AppearanceCfg::AppearanceCfg()
{
	this->backgroundColor = "#000000";
	this->channelActionColor = "#008000";
	this->defaultTextColor = "#b9b9b9";
	this->errorColor = "#ff0000";
	this->mainFont = QFont("Courier");
	this->networkActionColor = "#079CFF";
	this->userListFont = QFont("Courier");
}

void IRCConfig::AppearanceCfg::init(IniSection& section)
{
	section.createSetting("BackgroundColor", this->backgroundColor);	
	section.createSetting("ChannelActionColor", this->channelActionColor);
	section.createSetting("DefaultTextColor", this->defaultTextColor);
	section.createSetting("ErrorColor", this->errorColor);
	section.createSetting("MainFont", this->mainFont.toString());
	section.createSetting("NetworkActionColor", this->networkActionColor);
	section.createSetting("UserListFont", this->userListFont.toString());
}

void IRCConfig::AppearanceCfg::load(IniSection& section)
{
	this->backgroundColor = (const QString &)section["BackgroundColor"];
	this->channelActionColor = (const QString &)section["ChannelActionColor"];
	this->defaultTextColor = (const QString &)section["DefaultTextColor"];
	this->errorColor = (const QString &)section["ErrorColor"];
	this->mainFont.fromString(section["MainFont"]);
	this->networkActionColor = (const QString &)section["NetworkActionColor"];
	this->userListFont.fromString(section["UserListFont"]);
}

void IRCConfig::AppearanceCfg::save(IniSection& section)
{
	section["BackgroundColor"] = this->backgroundColor;
	section["ChannelActionColor"] = this->channelActionColor;
	section["DefaultTextColor"] = this->defaultTextColor;
	section["ErrorColor"] = this->errorColor;
	section["MainFont"] = this->mainFont.toString();
	section["NetworkActionColor"] = this->networkActionColor;
	section["UserListFont"] = this->userListFont.toString();
}

//////////////////////////////////////////////////////////////////////////////
const QString IRCConfig::GeneralCfg::SECTION_NAME = "General";

IRCConfig::GeneralCfg::GeneralCfg()
{
	
}

void IRCConfig::GeneralCfg::load(IniSection& section)
{
	
}

void IRCConfig::GeneralCfg::save(IniSection& section)
{
	
}

//////////////////////////////////////////////////////////////////////////////
const QString IRCConfig::NetworksDataCfg::SECTIONS_NAMES_PREFIX = "Network.";

void IRCConfig::NetworksDataCfg::clearNetworkSections(Ini& ini)
{
	QVector<IniSection*> sections = ini.sectionsArray("^" + SECTIONS_NAMES_PREFIX);
	foreach (IniSection* pSection, sections)
	{
		ini.deleteSection(pSection->sectionName());
	}
}
	
void IRCConfig::NetworksDataCfg::networksSortedByDescription(QVector<IRCNetworkEntity>& outVector)
{
	outVector = this->networks;
	qSort(outVector);
}
	
void IRCConfig::NetworksDataCfg::load(Ini& ini)
{
	QVector<IniSection*> sections = ini.sectionsArray("^" + SECTIONS_NAMES_PREFIX);
	foreach (IniSection* pSection, sections)
	{
		IniSection& iniSection = *pSection;
	
		IRCNetworkEntity network;
		network.address = (const QString &)iniSection["Address"];
		network.description = (const QString &)iniSection["Description"];
		network.nickservCommand = (const QString &)iniSection["NickservCommand"];
		network.nickservPassword = (const QString &)iniSection["NickservPassword"];
		network.password = (const QString &)iniSection["Password"];
		network.port = iniSection["Port"];
		
		this->networks << network;
	}

	// Go through the plugins and register their IRC servers.
	for(unsigned int i = 0;i < Main::enginePlugins->numPlugins();i++)
	{
		(*Main::enginePlugins)[i]->info->pInterface->registerIRCServer(this->networks);
	}
}

void IRCConfig::NetworksDataCfg::save(Ini& ini)
{
	// Erase all previously stored networks.
	// We need to rebuild these sections from scratch.
	clearNetworkSections(ini);

	for (int i = 0; i < this->networks.size(); ++i)
	{
		QString sectionName = SECTIONS_NAMES_PREFIX + QString::number(i);
		
		const IRCNetworkEntity& network = this->networks[i];
		
		IniSection& iniSection = ini.section(sectionName);
		iniSection["Address"] = network.address;
		iniSection["Description"] = network.description;
		iniSection["NickservCommand"] = network.nickservCommand;
		iniSection["NickservPassword"] = network.nickservPassword;
		iniSection["Password"] = network.password;
		iniSection["Port"] = network.port;
	}
}
//////////////////////////////////////////////////////////////////////////////
const QString IRCConfig::PersonalCfg::SECTION_NAME = "Personal";

IRCConfig::PersonalCfg::PersonalCfg()
{
}

void IRCConfig::PersonalCfg::load(IniSection& section)
{
	this->alternativeNickname = (const QString &)section["AlternativeNickname"];
	this->fullName = (const QString &)section["FullName"];
	this->nickname = (const QString &)section["Nickname"];
}

void IRCConfig::PersonalCfg::save(IniSection& section)
{
	section["AlternativeNickname"] = this->alternativeNickname;
	section["FullName"] = this->fullName;
	section["Nickname"] = this->nickname;
}
