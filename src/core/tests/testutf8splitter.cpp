//------------------------------------------------------------------------------
// testutf8splitter.cpp
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
#include "testutf8splitter.h"

#include "utf8splitter.h"
#include <QDebug>

#define T_ASSERT_EQUAL(a, b) \
if ((a) != (b)) \
{ \
	qDebug() << "a:" << (a) << ", b:" << (b); \
	return false; \
} 

bool TestUtf8Splitter::executeTest()
{
	{
		QString in;
		for (int i = 0; i < 500; ++i)
		{
			in.append(QChar(0x2194));
		}
		QList<QByteArray> result = Utf8Splitter().split(in.toUtf8(), 50);
		T_ASSERT_EQUAL(result.size(), 30);
	}
	{
		QString in;
		for (int i = 0; i < 500; ++i)
		{
			in.append(QChar(0x2194));
		}
		QList<QByteArray> result = Utf8Splitter().split(in.toUtf8(), 48);
		T_ASSERT_EQUAL(result.size(), 32);
		T_ASSERT_EQUAL(result[0].size(), 48);
		T_ASSERT_EQUAL(result[1].size(), 48);
		T_ASSERT_EQUAL(result.last().size(), 12);
	}
	{
		QString in;
		for (int i = 0; i < 500; ++i)
		{
			in.append(QChar(0x00F8));
		}
		QList<QByteArray> result = Utf8Splitter().split(in.toUtf8(), 11);
		T_ASSERT_EQUAL(result.size(), 91);
		T_ASSERT_EQUAL(result.last().size(), 10);
	}
	{
		QString in;
		for (int i = 0; i < 250; ++i)
		{
			in.append(QChar('a'));
			in.append(QChar(0x00F8));
		}
		QList<QByteArray> result = Utf8Splitter().split(in.toUtf8(), 11);
		T_ASSERT_EQUAL(result.size(), 69);
		T_ASSERT_EQUAL(result.last().size(), 2);
	}
	return true;
}
