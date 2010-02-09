//------------------------------------------------------------------------------
// strings.cpp
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
#include "strings.h"

#include <QStringList>

bool Strings::isCharOnCharList(char c, const QString& charList)
{
	for (int i = 0; i < charList.length(); ++i)
	{
		if (charList[i] == c)
			return true;
	}

	return false;
}

void Strings::translateServerAddress(const QString& addressString, QString& hostname, short& port, const QString& defaultHostname, const short defaultPort)
{
	port = 0;
	QStringList addressAndPort = addressString.split(":");
	
	if (addressAndPort.size() == 0 || addressAndPort.size() > 2)
	{ // if something is not right set default settings
		hostname = defaultHostname;
	}
	else
	{
		hostname = addressAndPort[0];
		if (addressAndPort.size() == 2)
		{
			port = addressAndPort[1].toShort();
		}
	}

	if (port == 0)
	{
		port = defaultPort;
	}
}

QString& Strings::trimr(QString& str, const QString& charList)
{
	int i;
	for (i = str.length() - 1; i >= 0; --i)
	{
		if (!isCharOnCharList(str[i].toAscii(), charList))
			break;
	}
	++i;

	return str.remove(i, str.length() - i);
}

QString& Strings::triml(QString& str, const QString& charList)
{
	int i;
	for (i = 0; i < str.length(); ++i)
	{
		if (!isCharOnCharList(str[i].toAscii(), charList))
			break;
	}

	return str.remove(0, i);
}