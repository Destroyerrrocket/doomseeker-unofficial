//------------------------------------------------------------------------------
// utf8splitter.cpp
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
		quint8 curByte;
		stream >> curByte;
		chunk.append(curByte);
		if ((curByte & 0x80) != 0)
		{
			quint8 leading = curByte & 0xfc;
			int numBytes = 0;
			while ((leading & 0x80) != 0)
			{
				leading = leading << 1;
				++numBytes;
			}
			// When sub-bytes are appended to the chunk
			// we already have 1 byte: the leading byte.
			for (; numBytes > 1; --numBytes)
			{
				quint8 subByte;
				stream >> subByte;
				chunk.append(subByte);
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
