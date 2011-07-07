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
#include "plugins/engineplugin.h"
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
	
bool IRCConfig::isAutojoinNetworksEnabled() const
{
	foreach (const IRCNetworkEntity& network, networks.networks)
	{
		if (network.bAutojoinNetwork)
		{
			return true;
		}
	}
	
	return false;
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
	
	pSection = &pIni->section(SoundsCfg::SECTION_NAME);
	sounds.load(*pSection);	
	
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
	
	pSection = &pIni->section(SoundsCfg::SECTION_NAME);
	sounds.save(*pSection);
	
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
	this->timestamps = true;
	this->userListFont = QFont("Courier");
	this->urlColor = "#00F6FF";
}

void IRCConfig::AppearanceCfg::init(IniSection& section)
{
	section.createSetting("BackgroundColor", this->backgroundColor);	
	section.createSetting("ChannelActionColor", this->channelActionColor);
	section.createSetting("DefaultTextColor", this->defaultTextColor);
	section.createSetting("ErrorColor", this->errorColor);
	section.createSetting("MainFont", this->mainFont.toString());
	section.createSetting("NetworkActionColor", this->networkActionColor);
	section.createSetting("TimeStamps", this->timestamps);
	section.createSetting("UserListFont", this->userListFont.toString());
	section.createSetting("UrlColor", this->urlColor);
}

void IRCConfig::AppearanceCfg::load(IniSection& section)
{
	this->backgroundColor = (const QString &)section["BackgroundColor"];
	this->channelActionColor = (const QString &)section["ChannelActionColor"];
	this->defaultTextColor = (const QString &)section["DefaultTextColor"];
	this->errorColor = (const QString &)section["ErrorColor"];
	this->mainFont.fromString(section["MainFont"]);
	this->networkActionColor = (const QString &)section["NetworkActionColor"];
	this->timestamps = section["TimeStamps"];
	this->userListFont.fromString(section["UserListFont"]);
	this->urlColor = (const QString &)section["UrlColor"];
}

void IRCConfig::AppearanceCfg::save(IniSection& section)
{
	section["BackgroundColor"] = this->backgroundColor;
	section["ChannelActionColor"] = this->channelActionColor;
	section["DefaultTextColor"] = this->defaultTextColor;
	section["ErrorColor"] = this->errorColor;
	section["MainFont"] = this->mainFont.toString();
	section["NetworkActionColor"] = this->networkActionColor;
	section["TimeStamps"] = this->timestamps;
	section["UserListFont"] = this->userListFont.toString();
	section["UrlColor"] = this->urlColor;
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

QVector<IRCNetworkEntity> IRCConfig::NetworksDataCfg::autojoinNetworks() const
{
	QVector<IRCNetworkEntity> autoNetworks;
	foreach (const IRCNetworkEntity& network, this->networks)
	{
		if (network.bAutojoinNetwork)
		{
			autoNetworks << network;
		}
	}
	
	return autoNetworks;
}

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
		
		this->loadNetwork(iniSection, network);
		
		this->networks << network;
	}
	
	IniSection& lastUsedNetworkSection = ini.section("LastUsedNetwork");
	this->loadNetwork(lastUsedNetworkSection, this->lastUsedNetwork);

	// Go through the plugins and register their IRC servers.
	for(unsigned int i = 0;i < Main::enginePlugins->numPlugins();i++)
	{
		(*Main::enginePlugins)[i]->info->registerIRCServer(this->networks);
	}
}

void IRCConfig::NetworksDataCfg::loadNetwork(const IniSection& iniSection, IRCNetworkEntity& network)
{
	network.address = (const QString &)iniSection["Address"];
	network.bAutojoinNetwork = iniSection["bAutojoinNetwork"];
	network.autojoinChannels = ((const QString &)iniSection["AutojoinChannels"]).split(" ", QString::SkipEmptyParts);
	network.description = (const QString &)iniSection["Description"];
	network.nickservCommand = (const QString &)iniSection["NickservCommand"];
	network.nickservPassword = (const QString &)iniSection["NickservPassword"];
	network.password = (const QString &)iniSection["Password"];
	network.port = iniSection["Port"];
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
		
		this->saveNetwork(iniSection, network);
	}
	
	IniSection& lastUsedNetworkSection = ini.section("LastUsedNetwork");
	this->saveNetwork(lastUsedNetworkSection, this->lastUsedNetwork);
}

void IRCConfig::NetworksDataCfg::saveNetwork(IniSection& iniSection, const IRCNetworkEntity& network)
{
	iniSection["Address"] = network.address;
	iniSection["bAutojoinNetwork"] = network.bAutojoinNetwork;
	iniSection["AutojoinChannels"] = network.autojoinChannels.join(" ");
	iniSection["Description"] = network.description;
	iniSection["NickservCommand"] = network.nickservCommand;
	iniSection["NickservPassword"] = network.nickservPassword;
	iniSection["Password"] = network.password;
	iniSection["Port"] = network.port;
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
//////////////////////////////////////////////////////////////////////////////
const QString IRCConfig::SoundsCfg::SECTION_NAME = "Sounds";

IRCConfig::SoundsCfg::SoundsCfg()
{
	this->bUseNicknameUsedSound = false;
	this->bUsePrivateMessageReceivedSound = false;
}

void IRCConfig::SoundsCfg::load(IniSection& section)
{
	this->bUseNicknameUsedSound = section["bUseNicknameUsedSound"];
	this->bUsePrivateMessageReceivedSound = section["bUsePrivateMessageReceivedSound"];
	this->nicknameUsedSound = (const QString&)section["NicknameUsedSound"];
	this->privateMessageReceivedSound = (const QString&)section["PrivateMessageReceivedSound"];
}

void IRCConfig::SoundsCfg::save(IniSection& section)
{
	section["bUseNicknameUsedSound"] = this->bUseNicknameUsedSound;
	section["bUsePrivateMessageReceivedSound"] = this->bUsePrivateMessageReceivedSound;
	section["NicknameUsedSound"] = this->nicknameUsedSound;
	section["PrivateMessageReceivedSound"] = this->privateMessageReceivedSound;
}
