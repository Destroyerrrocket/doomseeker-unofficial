//------------------------------------------------------------------------------
// testruns.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "testruns.h"
#include "tests/testdatapaths.h"
#include "tests/testini.h"

TestCore* TestRuns::pTestCore = NULL;
		
void TestRuns::callTests()
{
	pTestCore->executeTest(new TestDataPathsAppDataDirectoryAccess(false));
	pTestCore->executeTest(new TestDataPathsAppDataDirectoryAccess(true));
	
	pTestCore->executeTest(new TestDataPathsAppDataDirectoryWrite(false));
	pTestCore->executeTest(new TestDataPathsAppDataDirectoryWrite(true));
	
	// Note: All of these tests may fail if TestReadINI fails.
	pTestCore->executeTest(new TestReadINI());
	pTestCore->executeTest(new TestReadINIList());
	pTestCore->executeTest(new TestReadINIVariable());
	pTestCore->executeTest(new TestReadINIWithErrors());
	pTestCore->executeTest(new TestDeleteINISection());
	pTestCore->executeTest(new TestDeleteINIVariable());
}
