//------------------------------------------------------------------------------
// dmflagshtmlgenerator.cpp
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
#include "dmflagshtmlgenerator.h"

#include "serverapi/server.h"
#include "serverapi/serverstructs.h"

class DmflagsHtmlGenerator::PrivData
{
	public:
		ServerCPtr server;
};

DmflagsHtmlGenerator::DmflagsHtmlGenerator(const ServerCPtr &server)
{
	d = new PrivData();
	d->server = server;
}

DmflagsHtmlGenerator::~DmflagsHtmlGenerator()
{
	delete d;
}

QString DmflagsHtmlGenerator::generate()
{
	QString result;
	const QList<DMFlagsSection> sections = d->server->dmFlags();
	foreach(const DMFlagsSection &section, sections)
	{
		result += section.name() + ":\n";
		for(int i = 0; i < section.count(); ++i)
		{
			result += section[i].name() + '\n';
		}
		result += '\n';
	}
	return result;
}
