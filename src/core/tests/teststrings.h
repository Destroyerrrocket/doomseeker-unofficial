//------------------------------------------------------------------------------
// teststrings.h
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __TESTSTRINGS_H__
#define __TESTSTRINGS_H__

#include "tests/testbase.h"

class TestStringsWrapUrls : public TestUnitBase
{
	public:
		TestStringsWrapUrls()
		: TestUnitBase("Wrap URLs with A HREF tags")
		{
		}
	
		bool executeTest();
};

#endif
