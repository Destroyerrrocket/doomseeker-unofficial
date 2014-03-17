//------------------------------------------------------------------------------
// testruns.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "testruns.h"
#include "tests/testdatapaths.h"
#include "tests/testini.h"
#include "tests/testircisupportparser.h"
#include "tests/teststrings.h"
#include "tests/testutf8splitter.h"

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
	pTestCore->executeTest(new TestStringsWrapUrls());

	pTestCore->executeTest(new TestIRCISupportPrefix());
	pTestCore->executeTest(new TestIRCISupportNoPrefix());

	pTestCore->executeTest(new TestUtf8Splitter());
}
