//------------------------------------------------------------------------------
// teststrings.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "teststrings.h"
#include "strings.hpp"

bool TestStringsWrapUrls::executeTest()
{
	QString exampleString = "http://www.example.com www.example.com ftp://ftp.example.com www.\nwww http:// www.link.with.tag.com< http://x http://www.example.com?var1=1&var2=2#anchor www.no-spacebar.com";

	QString resultString = Strings::wrapUrlsWithHtmlATags(exampleString);
	testLog << "Result: " << resultString;
	return resultString.count("<a href=") == 7;
}
