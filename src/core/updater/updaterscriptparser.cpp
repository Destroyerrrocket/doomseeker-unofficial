//------------------------------------------------------------------------------
// updaterscriptparser.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "updaterscriptparser.h"

#include "autoupdater.h"
#include <QDebug>
#include <cassert>

class UpdaterScriptParser::PrivData
{
	public:
		QDomDocument* pDoc;
};
//////////////////////////////////////////////////////////////////////////////
UpdaterScriptParser::UpdaterScriptParser(QDomDocument& document)
{
	d = new PrivData();
	d->pDoc = &document;
}

UpdaterScriptParser::~UpdaterScriptParser()
{
	delete d;
}

QDomElement UpdaterScriptParser::mainElement()
{
	QDomNodeList nodes = d->pDoc->elementsByTagName("update");
	return nodes.at(0).toElement();
}

QString UpdaterScriptParser::msgMainElementMissingError()
{
	return AutoUpdater::tr("Missing main \"update\" node.");
}

QString UpdaterScriptParser::setPackageName(const QString& name)
{
	QString errorMsg;
	errorMsg = setPackageNameInPackages(name);
	if (errorMsg.isNull())
	{
		errorMsg = setPackageNameInFiles(name);
	}
	return errorMsg;
}

QString UpdaterScriptParser::setPackageNameInFiles(const QString& name)
{
	// Path: /"install"/"file"(*)/"package"/#text
	QDomNodeList nodes;
	QDomElement element;

	element = mainElement();
	if (element.isNull())
	{
		return msgMainElementMissingError();
	}
	element = element.elementsByTagName("install").at(0).toElement();
	if (element.isNull())
	{
		return AutoUpdater::tr("Missing \"install\" element.");
	}
	nodes = element.elementsByTagName("file");
	for (int i = 0; i < nodes.count(); ++i)
	{
		QDomNode textNode = nodes.at(i).toElement()
			.elementsByTagName("package").at(0).toElement().firstChild();
		textNode.setNodeValue(name);
		if (textNode.isNull())
		{
			return AutoUpdater::tr("Missing text node for \"package\" "
				"element for \"file\" element %1").arg(i);
		}
	}
	return QString();
}

QString UpdaterScriptParser::setPackageNameInPackages(const QString& name)
{
	// Path: /"packages"/"package"/"name"/#text
	QDomNodeList nodes;
	QDomElement element;

	element = mainElement();
	if (element.isNull())
	{
		return msgMainElementMissingError();
	}

	nodes = element.elementsByTagName("packages");
	if (nodes.isEmpty())
	{
		return AutoUpdater::tr("Missing \"packages\" element.");
	}
	element = nodes.at(0).toElement();
	// Now, Mendeley updater allows to have many packages defined in single
	// script file. Mendeley iterates over these packages and then uses
	// values of "name" elements to build filenames under which
	// packages archives are stored. Doomseeker will always use only one
	// package per script so if there is more than one element under "packages"
	// tag then this should be treated as an error in the script.
	nodes = element.elementsByTagName("package");
	if (nodes.size() != 1)
	{
		if (nodes.size() > 1)
		{
			return AutoUpdater::tr("More than one \"package\" element found.");
		}
		return false;
	}
	// Since we now know that there is only one "package" node
	// we can finally set its name.
	QDomNode nameTextNode = nodes.at(0).toElement()
		.elementsByTagName("name").at(0).toElement().firstChild();
	if (nameTextNode.isNull())
	{
		return AutoUpdater::tr("Failed to find \"name\" text node.");
	}
	return QString();
}

