//------------------------------------------------------------------------------
// chatnetworkscfg.h
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
#ifndef id6b479233_6426_4c75_892a_674e329b97f8
#define id6b479233_6426_4c75_892a_674e329b97f8

#include <QList>
#include <QString>

class Ini;
class IniSection;
class IRCNetworkEntity;

class ChatNetworksCfg
{
public:
	QList<IRCNetworkEntity> autoJoinNetworks() const;
	bool isAnyNetworkOnAutoJoin() const;

	IRCNetworkEntity lastUsedNetwork() const;
	void setLastUsedNetwork(const IRCNetworkEntity &network);

	QList<IRCNetworkEntity> networks() const;
	void setNetworks(const QList<IRCNetworkEntity> &networks);

private:
	static const QString SECTIONS_NAMES_PREFIX;

	QVector<IniSection> allNetworksSections() const;
	void clearNetworkSections();
	Ini& ini() const;
	IRCNetworkEntity loadNetwork(const IniSection &section) const;
	IniSection networkSection(int id);
	void saveNetwork(IniSection section, const IRCNetworkEntity& network);
};

#endif
