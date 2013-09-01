//------------------------------------------------------------------------------
// serverlistfilterinfo.cpp
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "serverlistfilterinfo.h"

ServerListFilterInfo::ServerListFilterInfo()
{
	bShowEmpty = true;
	bShowFull = true;
	bShowOnlyValid = false;
	maxPing = 0;
}

void ServerListFilterInfo::copy(const ServerListFilterInfo& other)
{
	bShowEmpty = other.bShowEmpty;
	bShowFull = other.bShowFull;
	bShowOnlyValid = other.bShowOnlyValid;
	gameModes = other.gameModes;
	maxPing = other.maxPing;
	serverName = other.serverName.trimmed();

	copyTrimmed(this->wads, other.wads);
	copyTrimmed(this->wadsExcluded, other.wadsExcluded);
}

void ServerListFilterInfo::copyTrimmed(QStringList& target, const QStringList& source) const
{
	target.clear();
	foreach (QString element, source)
	{
		element = element.trimmed();
		if (!element.isEmpty())
		{
			target << element;
		}
	}
}

bool ServerListFilterInfo::isFilteringAnything() const
{
	if (!bShowEmpty || !bShowFull)
	{
		return true;
	}

	if (bShowOnlyValid || maxPing > 0)
	{
		return true;
	}

	if (!gameModes.isEmpty()
	||  !serverName.isEmpty()
	||  !wads.isEmpty()
	||  !wadsExcluded.isEmpty())
	{
		return true;
	}

	return false;
}

QString ServerListFilterInfo::toString() const
{
	QString ret = "";

	ret += QString("bShowEmpty: ") + (bShowEmpty ? "Yes" : "No") + "\n";
	ret += QString("bShowFull: ") + (bShowFull ? "Yes" : "No") + "\n";
	ret += QString("bShowOnlyValid: ") + (bShowOnlyValid ? "Yes" : "No") + "\n";
	ret += QString("GameMode: ") + gameModes.join(",") + "\n";
	ret += QString("MaxPing: ") + QString::number(maxPing) + "\n";
	ret += QString("ServerName: ") + serverName + "\n";
	ret += QString("WADs: ") + wads.join(",") + "\n";
	ret += QString("WADs Excluded: ") + wads.join(",") + "\n";

	return ret;
}

