// Emacs style mode select   -*- C++ -*-
// =============================================================================
// ### ### ##   ## ###  #   ###  ##   #   #  ##   ## ### ##  ### ###  #  ###
// #    #  # # # # #  # #   #    # # # # # # # # # # #   # #  #   #  # # #  #
// ###  #  #  #  # ###  #   ##   # # # # # # #  #  # ##  # #  #   #  # # ###
//   #  #  #     # #    #   #    # # # # # # #     # #   # #  #   #  # # #  #
// ### ### #     # #    ### ###  ##   #   #  #     # ### ##  ###  #   #  #  #
//                                     --= http://bitowl.com/sde/ =--
// =============================================================================
// Copyright (C) 2008 "Blzut3" (admin@maniacsvault.net)
// The SDE Logo is a trademark of GhostlyDeath (ghostlydeath@gmail.com)
// =============================================================================
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
// =============================================================================
// Description:
// =============================================================================


#include "sdeapi/config.hpp"
#include "sdeapi/scanner.hpp"
#include "main.h"
#include "log.h"

#include <QFile>
#include <QDir>
#include <QDebug>

void SettingsData::setValue(int integer)
{
	if(m_type != ST_INT || m_integer != integer)
	{
		m_integer = integer;
		m_type = ST_INT;
		if (m_parent != NULL)
		{
			m_parent->settingsChanged = true;
		}
	}
}

void SettingsData::setValue(QString str)
{
	if(m_type != ST_STR || m_str != str)
	{
		m_str = str;
		m_type = ST_STR;
		if (m_parent != NULL)
		{
			m_parent->settingsChanged = true;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

Config::Config() : firstRun(false)
{
}

Config::Config(const QString& cfgFile, bool readCfg) : firstRun(false)
{
	configFile = cfgFile;

	if (readCfg)
	{
		readConfig();
	}
}

Config::~Config()
{
	for(QHash<QString, SettingsData *>::iterator it=settings.begin();it != settings.end();it++)
		delete it.value();
	settings.clear();
}

void Config::clear()
{
	foreach(SettingsData* set, settings)
	{
		delete set;
	}

	settings.clear();
}

void Config::locateConfigFile(int argc, char* argv[])
{
	QDir configDir;
#if defined(Q_OS_WIN32)
	configDir = Main::workingDirectory;
#else
	QDir home = QDir::home();
	if(!home.exists())
	{
		pLog << tr("Please set your HOME environment variable.");
		return;
	}
	configDir = home.absolutePath() + "/.doomseeker/";
	if(!home.exists(".doomseeker"))
	{
		if(!home.mkdir(".doomseeker"))
		{
			pLog << tr("Could not create settings directory, configuration will not be saved.");
			return;
		}
	}
#endif
	configFile = configDir.absolutePath() + "/doomseeker.cfg";
	pLog << tr("Config file is: %1").arg(configFile);

	readConfig();
}

// NOTE: Be sure that '\\' is the first thing in the array otherwise it will re-escape.
static char escapeCharacters[] = {'\\', '"', 0};
const QString& Config::escape(QString &str)
{
	for(unsigned int i = 0;escapeCharacters[i] != 0;i++)
	{
		// += 2 because we'll be inserting 1 character.
		for(int p = 0;p < str.length() && (p = str.indexOf(escapeCharacters[i], p)) != -1;p += 2)
		{
			str.insert(p, '\\');
		}
	}
	return str;
}
const QString& Config::unescape(QString &str)
{
	for(unsigned int i = 0;escapeCharacters[i] != 0;i++)
	{
		QString sequence = "\\" + QString(escapeCharacters[i]);
		for(int p = 0;p < str.length() && (p = str.indexOf(sequence, p)) != -1;p++)
			str.replace(str.indexOf(sequence, p), 2, escapeCharacters[i]);
	}
	return str;
}

void Config::readConfig()
{
	// Check to see if we have located the config file.
	if(configFile.isEmpty())
		return;

	QFile stream(configFile);
	if(stream.open(QIODevice::ReadOnly))
	{
		qint64 size = stream.size();
		char* data = new char[size];
		if(stream.read(data, size) == -1)
		{
			delete[] data;
			return;
		}
		stream.close();

		Scanner sc(data, size);
		while(sc.tokensLeft())  // Go until there is nothing left to read.
		{
			sc.mustGetToken(TK_Identifier);
			QString index = sc.str;
			sc.mustGetToken('=');
			if(sc.checkToken(TK_StringConst))
			{
				createSetting(index, "");
				setting(index)->setValue(sc.str);
			}
			else
			{
				bool negative = sc.checkToken('-');
				sc.mustGetToken(TK_IntConst);
				createSetting(index, 0);
				setting(index)->setValue(negative ? -sc.number : sc.number);
			}
			sc.mustGetToken(';');
		}

		delete[] data;
	}

	if(settings.size() == 0)
		firstRun = true;

	// Make sure we're clear that nothing has changed.
	settingsChanged = false;
}

bool Config::saveConfig()
{
	// No real reason to do anything.
	if(!settingsChanged)
		return true;

	// Check to see if we're saving the settings.
	if(configFile.isEmpty())
		return false;

	pLog << tr("Saving configuration file: \"%1\"").arg(configFile);

	QFile stream(configFile);
	if(stream.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		for(QHash<QString, SettingsData *>::iterator it=settings.begin();it != settings.end();it++)
		{
			if(stream.write(it.key().toAscii()) == -1)
				return false;

			SettingsData *data = it.value();
			if(data->type() == SettingsData::ST_INT)
			{
				QString value = QString(" = %1;\n").arg(data->integer());
				qint64 error = stream.write(value.toAscii().constData(), value.length());
				if(error == -1)
					return false;
			}
			else
			{
				QString str = data->string(); // Make a non const copy of the string.
				escape(str);
				QString value = QString(" = \"%1\";\n").arg(str);
				qint64 error = stream.write(value.toAscii().constData(), value.length());
				if(error == -1)
					return false;
			}
		}
		stream.close();
		settingsChanged = false;
		return true;
	}

	return false;

}

void Config::createSetting(const QString index, unsigned int defaultInt)
{
	SettingsData* data;
	if(!findIndex(index, data))
	{
		data = new SettingsData(this, defaultInt);
		settings[index] = data;
	}
}

void Config::createSetting(const QString index, QString defaultString)
{
	SettingsData* data;
	if(!findIndex(index, data))
	{
		data = new SettingsData(this, defaultString);
		settings[index] = data;

		// Modified
		settingsChanged = true;
	}
}

SettingsData* Config::setting(const QString index)
{
	SettingsData *data;
	if(!findIndex(index, data))
	{
		createSetting(index, "");
		findIndex(index, data);
	}
	return data;
}

bool Config::settingExists(const QString& index)
{
	SettingsData *data;
	if(!findIndex(index, data))
	{
		return false;
	}
	return true;
}

bool Config::findIndex(const QString index, SettingsData *&data)
{
	QHash<QString, SettingsData *>::iterator it = settings.find(index);
	if(it != settings.end())
	{
		data = it.value();
		return true;
	}
	return false;
}

