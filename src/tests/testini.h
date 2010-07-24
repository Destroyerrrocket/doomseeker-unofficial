//------------------------------------------------------------------------------
// testini.h
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
#ifndef __TESTINI_H__
#define __TESTINI_H__

#include "tests/testbase.h"
#include <QByteArray>

class ExampleINIs
{
	public:
		/**
		 *	@brief Gets an INI file that is written in a format that is 100%
		 *	supported by the Ini class.
		 *
		 *	In other words: attempt to read this INI file should end with no
		 *	errors.
		 */
		static QByteArray	getExampleINI();
		
		/**
		 *	@brief Ini class should detect errors in this example.
		 */
		static QByteArray	getExampleError1();
};

class TestReadINI : public TestUnitBase
{
	public:
		TestReadINI();
	
		bool				executeTest();
};

class TestReadINIVariable : public TestUnitBase
{
	public:	
		TestReadINIVariable()
		: TestUnitBase("Read INI Variable")
		{
		}
		
		bool				executeTest();
};

class TestReadINIList : public TestUnitBase
{
	public:	
		TestReadINIList()
		: TestUnitBase("Read INI List")
		{
		}
		
		bool				executeTest();
		
	protected:
		bool				compareEntry(const QString& actual, const QString& expected);
};

class TestDeleteINIVariable : public TestUnitBase
{
	public:	
		TestDeleteINIVariable()
		: TestUnitBase("Delete INI Variable")
		{
		}
		
		bool				executeTest();
};

class TestDeleteINISection : public TestUnitBase
{
	public:	
		TestDeleteINISection()
		: TestUnitBase("Delete INI Section")
		{
		}
		
		bool				executeTest();
};

class TestReadINIWithErrors : public TestUnitBase
{
	public:	
		TestReadINIWithErrors()
		: TestUnitBase("Read INI file with errors")
		{
		}
		
		bool				executeTest();
};

#endif
