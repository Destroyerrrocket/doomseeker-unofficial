//------------------------------------------------------------------------------
// generalinfotip.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "generalinfotip.h"

#include "ip2c/ip2c.h"
#include "serverapi/server.h"

DClass<GeneralInfoTip>
{
	public:
		ServerCPtr server;
};

DPointered(GeneralInfoTip)

GeneralInfoTip::GeneralInfoTip(const ServerCPtr &server)
{
	d->server = server;
}

GeneralInfoTip::~GeneralInfoTip()
{
}

QString GeneralInfoTip::generateHTML()
{
	QString ret;
	if (d->server->isKnown())
	{
		ret += QString(d->server->name()).replace('>', "&gt;").replace('<', "&lt;") + "\n";
		ret += labelString(tr("Version"), d->server->gameVersion());
		ret += labelString(tr("E-mail"), d->server->email());
		ret += labelString(tr("URL"), d->server->webSite());
	}

	IP2CCountryInfo countryInfo = IP2C::instance()->obtainCountryInfo(d->server->address());

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
