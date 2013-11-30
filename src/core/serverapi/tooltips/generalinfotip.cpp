//------------------------------------------------------------------------------
// generalinfotip.cpp
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
#include "generalinfotip.h"

#include "ip2c/ip2c.h"
#include "serverapi/server.h"
#include "main.h"

GeneralInfoTip::GeneralInfoTip(const Server* server)
: pServer(server)
{
}

QString GeneralInfoTip::generateHTML()
{
	QString ret;
	if (pServer->isKnown())
	{
		ret += QString(pServer->name()).replace('>', "&gt;").replace('<', "&lt;") + "\n";
		ret += labelString(tr("Version"), pServer->gameVersion());
		ret += labelString(tr("E-mail"), pServer->email());
		ret += labelString(tr("URL"), pServer->webSite());
	}

	IP2CCountryInfo countryInfo = Main::ip2c->obtainCountryInfo(pServer->address());

	if (countryInfo.isValid())
	{
		if (!ret.isEmpty())
		{
			ret += "\n";
		}

		ret += tr("Location: %1\n").arg(countryInfo.name);
	}

	return ret;
}

QString GeneralInfoTip::labelString(QString label, QString valueString)
{
	if (valueString.isEmpty())
	{
		return QString();
	}
	else
	{
		return label + ": " + valueString + "\n";
	}
}
