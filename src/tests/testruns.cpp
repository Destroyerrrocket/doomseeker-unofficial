//------------------------------------------------------------------------------
// testruns.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "testruns.h"
#include "tests/testdatapaths.h"

TestCore* TestRuns::pTestCore = NULL;
		
void TestRuns::callTests()
{
	pTestCore->executeTest(new TestDataPathsHomeDirectoryAccess(false));
	pTestCore->executeTest(new TestDataPathsHomeDirectoryAccess(true));
}
