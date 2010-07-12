//------------------------------------------------------------------------------
// testcore.cpp
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
#include "testcore.h"
#include <QTime>

TestCore::TestCore()
{
	clearCounters();
}

void TestCore::clearCounters()
{
	testsFailed = 0;
	testsSucceeded = 0;
}

void TestCore::executeTest(TestUnitBase* pTest)
{
	testLog << "--------------------------------------------------------";
	testLog << QString("Test: %1").arg(pTest->testName());
	testLog << "--------------------------------------------------------";
	
	QTime time;
	time.start();
	
	bool bResult = pTest->executeTest();
	delete pTest;
	
	int executionTime = time.elapsed();
	
	QString resultString = "Result: %1. Time: %2 ms";
	resultString = resultString.arg(bResult ? "SUCCESS" : "FAIL").arg(executionTime);
	
	testLog << "--------------------------------------------------------";
	testLog << resultString;
	testLog << "--------------------------------------------------------";
	testLog << "";
	
	if (bResult)
	{
		++testsSucceeded;
	}
	else
	{
		++testsFailed;
	}
}
