//------------------------------------------------------------------------------
// chatnetworkscfg.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "chatnetworkscfg.h"

#include "irc/configuration/ircconfig.h"
#include "ini/inisection.h"
#include <QString>

const QString ChatNetworksCfg::SECTIONS_NAMES_PREFIX = "Network.";


QList<IRCNetworkEntity> ChatNetworksCfg::autoJoinNetworks() const
{
	QList<IRCNetworkEntity> result;
	foreach (const IRCNetworkEntity& network, networks())
	{
		if (network.isAutojoinNetwork())
		{
			result << network;
		}
	}

	return result;
}

bool ChatNetworksCfg::isAnyNetworkOnAutoJoin() const
{
	foreach (const IRCNetworkEntity& network, networks())
	{
		if (network.isAutojoinNetwork())
		{
			return true;
		}
	}

	return false;
}

IRCNetworkEntity ChatNetworksCfg::lastUsedNetwork() const
{
	return loadNetwork(ini().section("LastUsedNetwork"));
}

void ChatNetworksCfg::setLastUsedNetwork(const IRCNetworkEntity &network)
{
	saveNetwork(ini().section("LastUsedNetwork"), network);
}

QList<IRCNetworkEntity> ChatNetworksCfg::networks() const
{
	QList<IRCNetworkEntity> result;
	foreach (const IniSection &section, allNetworksSections())
	{
		result << loadNetwork(section);
	}
	return result;
}

void ChatNetworksCfg::setNetworks(const QList<IRCNetworkEntity> &networks)
{
	// Erase all previously stored networks.
	// We need to rebuild these sections from scratch.
	clearNetworkSections();
	int id = 0;
	foreach (const IRCNetworkEntity &network, networks)
	{
		saveNetwork(networkSection(id), network);
		++id;
	}
}

void ChatNetworksCfg::clearNetworkSections()
{
	foreach (const IniSection& section, allNetworksSections())
	{
		ini().deleteSection(section.name());
	}
}

IRCNetworkEntity ChatNetworksCfg::loadNetwork(const IniSection& section) const
{
	IRCNetworkEntity network;
	network.setAddress(section["Address"]);
	network.setAutojoinNetwork(section["bAutojoinNetwork"]);
	network.setAutojoinChannels(static_cast<QString>(section["AutojoinChannels"])
		.split(" ", QString::SkipEmptyParts));
	network.setAutojoinCommands(section.value("AutojoinCommands").toStringList());
	network.setDescription(section["Description"]);
	network.setNickservCommand(section["NickservCommand"]);
	network.setNickservPassword(section["NickservPassword"]);
	network.setPassword(section["Password"]);
	network.setPort(section["Port"]);
	return network;
}

void ChatNetworksCfg::saveNetwork(IniSection section, const IRCNetworkEntity& network)
{
	section["Address"] = network.address();
	section["bAutojoinNetwork"] = network.isAutojoinNetwork();
	section["AutojoinChannels"] = network.autojoinChannels().join(" ");
	section["AutojoinCommands"].setValue(network.autojoinCommands());
	section["Description"] = network.description();
	section["NickservCommand"] = network.nickservCommand();
	section["NickservPassword"] = network.nickservPassword();
	section["Password"] = network.password();
	section["Port"] = network.port();
}

QVector<IniSection> ChatNetworksCfg::allNetworksSections() const
{
	return ini().sectionsArray("^" + SECTIONS_NAMES_PREFIX);
}

IniSection ChatNetworksCfg::networkSection(int id)
{
	return ini().section(SECTIONS_NAMES_PREFIX + id);
}

Ini& ChatNetworksCfg::ini() const
{
	return *gIRCConfig.ini();
}
