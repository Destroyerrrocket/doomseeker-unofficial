//------------------------------------------------------------------------------
// utf8splitter.cpp
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
#include "utf8splitter.h"

#include <QDataStream>

QList<QByteArray> Utf8Splitter::split(const QByteArray &in, int maxChunkLength)
{
	QList<QByteArray> result;
	QDataStream stream(in);
	QByteArray largerChunk;
	while (!stream.atEnd())
	{
		QByteArray chunk;
		quint8 i;
		stream >> i;
		if ((i & 0x1000000) == 0)
		{
			chunk.append(i);
		}
		else
		{
			quint8 leading = i & 0x11111100;
			int countZeros = 0;
			while ((leading & 1) == 0)
			{
				leading = leading >> 1;
				++countZeros;
			}
			int numBytes = (8 - countZeros) - 1;
			chunk.append(i);
			for (; numBytes > 0; --numBytes) {
				stream >> i;
				chunk.append(i);
			}
		}
		if (largerChunk.size() + chunk.size() > maxChunkLength)
		{
			result << largerChunk;
			largerChunk.clear();
		}
		largerChunk.append(chunk);
	}
	if (!largerChunk.isEmpty())
	{
		result << largerChunk;
	}
	return result;
}
