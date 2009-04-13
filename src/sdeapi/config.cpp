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

#include <fstream>
#include <stdlib.h>
#include <math.h>

#ifdef WINDOWS
#include <direct.h>
#define mkdir(file,mode) _mkdir(file)
#else
#include <sys/stat.h>
#endif

using namespace std;

Config::Config() : firstRun(false)
{
}

Config::~Config()
{
	for(map<QString, SettingsData *>::iterator it=settings.begin();it != settings.end();it++)
		delete (*it).second;
	settings.clear();
}

void Config::locateConfigFile(int argc, char* argv[])
{
	QString configDir;
#ifdef WINDOWS
	configDir = argv[0];
	UInt32 pos = static_cast<UInt32> (configDir.lastIndexOf('\\')) > static_cast<UInt32> (configDir.lastIndexOf('/')) ? configDir.lastIndexOf('\\') : configDir.lastIndexOf('/');
	configDir = configDir.left(pos+1);
#else
	char *home = getenv("HOME");
	if(home == NULL || *home == '\0')
	{
		printf("Please set your HOME environment variable.\n");
		return;
	}
	configDir = QString(home) + tr("/.doomseeker/");
	struct stat dirStat;
	if(stat(configDir.toAscii().constData(), &dirStat) == -1)
	{
		if(mkdir(configDir.toAscii().constData(), S_IRWXU) == -1)
		{
			printf("Could not create settings directory, configuration will not be saved.\n");
			return;
		}
	}
#endif
	configFile = configDir + tr("doomseeker.cfg");

	readConfig();
}

// NOTE: Be sure that '\\' is the first thing in the array otherwise it will re-escape.
static char escapeCharacters[] = {'\\', '"', 0};
const QString& Config::escape(QString &str)
{
	for(UInt32 i = 0;escapeCharacters[i] != 0;i++)
	{
		// += 2 because we'll be inserting 1 character.
		for(size_t p = 0;p < str.length() && (p = str.indexOf(escapeCharacters[i], p)) != static_cast<UInt32>(-1);p += 2)
		{
			str.insert(p, '\\');
		}
	}
	return str;
}

void Config::readConfig()
{
	// Check to see if we have located the config file.
	if(configFile.isEmpty())
		return;

	fstream stream(configFile.toAscii().constData(), ios_base::in | ios_base::binary);
	if(stream.is_open())
	{
		stream.seekg(0, ios_base::end);
		if(stream.fail())
			return;
		UInt32 size = stream.tellg();
		stream.seekg(0, ios_base::beg);
		if(stream.fail())
			return;
		char* data = new char[size];
		stream.read(data, size);
		// The eof flag seems to trigger fail on windows.
		if(!stream.eof() && stream.fail())
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
				sc.mustGetToken(TK_IntConst);
				createSetting(index, 0);
				setting(index)->setValue(sc.number);
			}
			sc.mustGetToken(';');
		}

		delete[] data;
	}

	if(settings.size() == 0)
		firstRun = true;
}

void Config::saveConfig()
{
	// Check to see if we're saving the settings.
	if(configFile.isEmpty())
		return;

	fstream stream(configFile.toAscii().constData(), ios_base::out | ios_base::trunc);
	if(stream.is_open())
	{
		for(map<QString, SettingsData *>::iterator it=settings.begin();it != settings.end();it++)
		{
			stream.write((*it).first.toAscii().constData(), (*it).first.length());
			if(stream.fail())
				return;
			SettingsData *data = (*it).second;
			if(data->type() == SettingsData::ST_INT)
			{
				// Determine size of number.
				UInt32 intLength = 0;
				do
				{
					intLength++;
				}
				while(data->integer()/static_cast<UInt32>(pow(10.0, static_cast<double>(intLength))) != 0);

				char* value = new char[intLength + 6];
				sprintf(value, " = %d;\n", data->integer());
				stream.write(value, intLength + 5);
				delete[] value;
				if(stream.fail())
					return;
			}
			else
			{
				QString str = data->string(); // Make a non const copy of the string.
				escape(str);
				char* value = new char[str.length() + 8];
				sprintf(value, " = \"%s\";\n", str.toAscii().constData());
				stream.write(value, str.length() + 7);
				delete[] value;
				if(stream.fail())
					return;
			}
		}
		stream.close();
	}
}

void Config::createSetting(const QString index, UInt32 defaultInt)
{
	SettingsData* data;
	if(!findIndex(index, data))
	{
		data = new SettingsData(defaultInt);
		settings[index] = data;
	}
}

void Config::createSetting(const QString index, QString defaultString)
{
	SettingsData* data;
	if(!findIndex(index, data))
	{
		data = new SettingsData(defaultString);
		settings[index] = data;
	}
}

SettingsData* Config::setting(const QString index)
{
	SettingsData *data;
	findIndex(index, data);
	return data;
}

bool Config::findIndex(const QString index, SettingsData *&data)
{
	map<QString, SettingsData *>::iterator it = settings.find(index);
	if(it != settings.end())
	{
		data = (*it).second;
		return true;
	}
	return false;
}

