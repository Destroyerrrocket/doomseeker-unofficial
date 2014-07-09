//------------------------------------------------------------------------------
// testruns.h
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __TESTRUNS_H__
#define __TESTRUNS_H__

#include "tests/testcore.h"

/**
 *	@brief Use this class to execute test runs.
 */
class TestRuns
{
	public:
		static TestCore* pTestCore;

		static void callTests();
};

#endif
