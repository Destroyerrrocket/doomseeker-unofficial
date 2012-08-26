//------------------------------------------------------------------------------
// datastreamoperatorwrapper.h
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
#include "datastreamoperatorwrapper.h"

#include "strings.h"
#include <cassert>

#define RETTYPE(type) \
type tmp; \
(*s) >> tmp; \
return tmp;

DataStreamOperatorWrapper::DataStreamOperatorWrapper(QDataStream* stream)
{
	assert(stream != NULL);
	this->s = stream;
}

qint8 DataStreamOperatorWrapper::readQInt8()
{
	RETTYPE(qint8);
}

bool DataStreamOperatorWrapper::readBool()
{
	RETTYPE(bool);
}

quint8 DataStreamOperatorWrapper::readQUInt8()
{
	RETTYPE(quint8);
}

quint16 DataStreamOperatorWrapper::readQUInt16()
{
	RETTYPE(quint16);
}

qint16 DataStreamOperatorWrapper::readQInt16()
{
	RETTYPE(qint16);
}

quint32 DataStreamOperatorWrapper::readQUInt32()
{
	RETTYPE(quint32);
}

qint32 DataStreamOperatorWrapper::readQInt32()
{
	RETTYPE(qint32);
}

quint64 DataStreamOperatorWrapper::readQUInt64()
{
	RETTYPE(quint64);
}

qint64 DataStreamOperatorWrapper::readQInt64()
{
	RETTYPE(qint64);
}

float DataStreamOperatorWrapper::readFloat()
{
	RETTYPE(float);
}

double DataStreamOperatorWrapper::readDouble()
{
	RETTYPE(double);
}

QByteArray DataStreamOperatorWrapper::readRawUntilByte(char stopByte)
{
	return Strings::readUntilByte(*s, stopByte);
}

qint64 DataStreamOperatorWrapper::remaining() const
{
	return s->device()->size() - s->device()->pos();
}

int DataStreamOperatorWrapper::skipRawData(int len)
{
	return s->skipRawData(len);
}
