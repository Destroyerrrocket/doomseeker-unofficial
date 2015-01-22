//------------------------------------------------------------------------------
// zandronumserverdmflagsparser.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "zandronumserverdmflagsparser.h"

#include "zandronumdmflags.h"
#include "zandronumgameinfo.h"
#include "zandronumserver.h"
#include <serverapi/serverstructs.h>
#include <datastreamoperatorwrapper.h>

ZandronumServerDmflagsParser *ZandronumServerDmflagsParser::mkParser(
	ZandronumServer *server, QDataStream *in)
{
	return new ZandronumServer2point0DmflagsParser(server, in);
}

ZandronumServerDmflagsParser::ZandronumServerDmflagsParser(
	ZandronumServer *server, QDataStream *in)
{
	this->server = server;
	this->inStream = in;
}

QList<DMFlagsSection> ZandronumServerDmflagsParser::sequential32Parse(
	const QList<DMFlagsSection> &knownFlags)
{
	DataStreamOperatorWrapper in(inStream);
	int numSections = in.readQInt8();

	QList<DMFlagsSection> result;
	// Read each dmflags section separately.
	for (int i = 0; i < knownFlags.count() && i < numSections; ++i)
	{
		unsigned int dmflags = in.readQInt32();

		const DMFlagsSection& knownSection = knownFlags[i];
		DMFlagsSection detectedSection(knownSection.name());

		// Iterate through every known flag to check whether it should be
		// inserted into the structure of this server.
		for (int j = 0; j < knownSection.count(); ++j)
		{
			if ( (dmflags & knownSection[j].value()) != 0)
			{
				detectedSection << knownSection[j];
			}
		}

		result << detectedSection;
	}
	if (knownFlags.count() < numSections)
	{
		// Skip all sections we didn't read.
		int remaining = numSections - knownFlags.count();
		in.skipRawData(remaining * 4);
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////////

QList<DMFlagsSection> ZandronumServerNullParser::parse()
{
	DataStreamOperatorWrapper in(inStream);
	int numSections = in.readQInt8();
	in.skipRawData(numSections * 4);
	return QList<DMFlagsSection>();
}

///////////////////////////////////////////////////////////////////////////////

QList<DMFlagsSection> ZandronumServer2point0DmflagsParser::parse()
{
	return sequential32Parse(ZandronumDmflags().flags());
}
