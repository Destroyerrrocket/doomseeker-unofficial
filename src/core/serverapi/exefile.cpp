//------------------------------------------------------------------------------
// exefile.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "exefile.h"

#include "configuration/doomseekerconfig.h"
#include "ini/inisection.h"
#include "ini/inivariable.h"
#include "message.h"
#include "plugins/engineplugin.h"

#include <QString>
#include <QFileInfo>

DClass<ExeFile>
{
	public:
		QString configKey;
		QString exeTypeName;
		QString programName;
};

DPointered(ExeFile)

ExeFile::ExeFile()
{
}

ExeFile::~ExeFile()
{
}

const QString& ExeFile::configKey() const
{
	return d->configKey;
}

const QString& ExeFile::exeTypeName() const
{
	return d->exeTypeName;
}

Message ExeFile::install(QWidget *parent)
{
	return Message();
}

QString ExeFile::pathToExe(Message& message)
{
	IniSection config = gConfig.iniSectionForPlugin(programName());
	IniVariable setting = config[configKey()];

	message = Message();
	QString path = setting.value().toString();
	QString error = "";
	if (path.trimmed().isEmpty())
	{
		error = tr("No %1 executable specified for %2").arg(exeTypeName()).arg(programName());
		message = Message::customError(error);
		return QString();
	}

	QFileInfo fi(path);

	if (!fi.exists() || (fi.isDir() && !fi.isBundle()))
	{
		// Remember: if the file doesn't exist
		// canonicalFilePath() will return an empty string!
		error = tr("Executable for %1 %2:\n%3\nis a directory or doesn't exist.")
			.arg(programName()).arg(exeTypeName())
			.arg(fi.absoluteFilePath());
		message = Message::customError(error);
		return QString();
	}

	return fi.absoluteFilePath();
}

const QString& ExeFile::programName() const
{
	return d->programName;
}

void ExeFile::setConfigKey(const QString& keyName)
{
	d->configKey = keyName;
}

void ExeFile::setExeTypeName(const QString& name)
{
	d->exeTypeName = name;
}

void ExeFile::setProgramName(const QString& name)
{
	d->programName = name;
}

QString ExeFile::workingDirectory(Message& message)
{
	QFileInfo fi(pathToExe(message));
	return fi.absolutePath();
}

