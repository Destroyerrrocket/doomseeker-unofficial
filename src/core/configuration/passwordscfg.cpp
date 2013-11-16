//------------------------------------------------------------------------------
// passwordscfg.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "passwordscfg.h"

#include "configuration/doomseekerconfig.h"
#include "configuration/serverpassword.h"
#include "ini/ini.h"
#include "ini/inisection.h"
#include "ini/inivariable.h"

const QString SECTION_NAME = "Passwords";

const QString SERVER_PASSWORDS_KEY = "ServerPasswords";

class PasswordsCfg::PrivData
{
	public:
		IniSection section;
};

PasswordsCfg::PasswordsCfg()
{
	d = new PrivData();
	d->section = gConfig.ini()->section(SECTION_NAME);
}

PasswordsCfg::~PasswordsCfg()
{
	delete d;
}

void PasswordsCfg::removeServerPhrase(const QString& phrase)
{
	QList<ServerPassword> allPasswords = serverPasswords();
	QMutableListIterator<ServerPassword> it(allPasswords);
	while (it.hasNext())
	{
		ServerPassword existingPass = it.next();
		if (existingPass.phrase() == phrase)
		{
			it.remove();
		}
	}
	setServerPasswords(allPasswords);
}

void PasswordsCfg::saveServerPhrase(const QString& phrase, const QString& serverName,
	const QString& engineName)
{
	ServerPassword pass;
	pass.setPhrase(phrase);
	pass.setLastGame(engineName);
	pass.setLastServer(serverName);
	pass.setLastTime(QDateTime::currentDateTime());
	removeServerPhrase(phrase);
	QList<ServerPassword> allPasswords = serverPasswords();
	allPasswords << pass;
	setServerPasswords(allPasswords);
}

QList<ServerPassword> PasswordsCfg::serverPasswords() const
{
	QList<ServerPassword> result;
	QVariantList vars = d->section[SERVER_PASSWORDS_KEY].value().toList();
	foreach (const QVariant& var, vars)
	{
		result << ServerPassword::deserializeQVariant(var);
	}
	return result;
}

QStringList PasswordsCfg::serverPhrases() const
{
	QStringList result;
	foreach (const ServerPassword& pass, serverPasswords())
	{
		result << pass.phrase();
	}
	return result;
}

void PasswordsCfg::setServerPasswords(const QList<ServerPassword>& val)
{
	QVariantList vars;
	foreach (const ServerPassword obj, val)
	{
		vars << obj.serializeQVariant();
	}
	d->section.setValue(SERVER_PASSWORDS_KEY, vars);
}
