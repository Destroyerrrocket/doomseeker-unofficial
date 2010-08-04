//------------------------------------------------------------------------------
// testini.cpp
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
#include "testini.h"
#include "ini.h"

#define EXAMPLE_INI_FILE \
"#This is a top comment \n\
#of an .INI file \n\
\n\
\n\
#This is a section comment \n\
#And another line of section comment \n\
[Section.SectionOne] \n\
Key1 = 10 \n\
Key2 = Value \n\
Key3 = \"A long text value\" \n\
#A top comment for a key \n\
CommentedKey = 1 #A side comment for a key\n\
\n\
[Section.List] \n\
A \n\
B \n\
C \n\
D \n\
"

#define EXAMPLE_ERROR_1 \
"[Unclosed section \n\
Key1 = key1 \n\
"

QByteArray ExampleINIs::getExampleINI()
{
	return QByteArray(EXAMPLE_INI_FILE);
}

QByteArray ExampleINIs::getExampleError1()
{
	return QByteArray(EXAMPLE_ERROR_1);
}

////////////////////////////////////////////////////////////////////////////////

TestReadINI::TestReadINI()
: TestUnitBase("Read INI")
{
}

bool TestReadINI::executeTest()
{
	Ini ini("ExampleINI", ExampleINIs::getExampleINI());
	
	const QStringList& errors = ini.errors();
	if (!errors.isEmpty())
	{
		testLog << "Errors were detected in a correct INI file:";
		foreach (QString error, errors)
		{
			testLog << QString("\t%1").arg(error);
		}
		return false;
	}
	
	// This should disregard characters case.
	IniSection* pSection = ini.section("section.sectionone");
	if (pSection == NULL)
	{
		testLog << "Section.SectionOne was not read correctly from the INI file.";
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool TestReadINIVariable::executeTest()
{
	Ini ini("ExampleINI", ExampleINIs::getExampleINI());

	IniVariable &pVariable = ini.retrieveSetting("section.sectionone", "key1");
	
	if (pVariable.isNull())
	{
		gLog << "Failed to obtain key.";
		return false;
	}
	
	int varValue = pVariable;
	
	/*if (pVariable->key.compare("Key1", Qt::CaseSensitive) != 0)
	{
		gLog << QString("Key name incorrect, expected 'Key1', got '%1'").arg(pVariable->key);
		return false;
	}*/
	
	if (varValue != 10)
	{
		gLog << QString("Value incorrect, expected '10', got '%1'").arg(varValue);
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool TestReadINIList::compareEntry(const QString& actual, const QString& expected)
{
	if (actual.compare(expected, Qt::CaseSensitive) != 0)
	{
		gLog << QString("List entry incorrect, expected '%1', got '%2'").arg(expected).arg(actual);
		return false;
	}
	
	return true;
}

bool TestReadINIList::executeTest()
{
	const int EXPECTED_LIST_SIZE = 4;

	Ini ini("ExampleINI", ExampleINIs::getExampleINI());
	
	IniSection *pSection = ini.section("section.list");
	
	QVector<IniVariable>& nameList = pSection->nameList;
	
	int listSize = nameList.size();
	
	if (listSize != EXPECTED_LIST_SIZE)
	{
		gLog << QString("List size incorrect, expected '%1', got '%2'").arg(EXPECTED_LIST_SIZE).arg(listSize);
		return false;
	}
	
	for (int i = 0; i < EXPECTED_LIST_SIZE; ++i)
	{
		if ( !compareEntry(nameList[i].value, QString('A' + i)) )
		{
			return false;
		}
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool TestDeleteINIVariable::executeTest()
{
	Ini ini("ExampleINI", ExampleINIs::getExampleINI());
	
	if (ini.retrieveSetting("section.sectionone", "key1").isNull())
	{
		gLog << "Variable doesn't exist already!";
		return false;
	}
	
	// Another way of removing a variable is to delete it directly from the
	// Ini file through Ini::deleteSetting(). Here we remove it from the 
	// section. This works the same and is provided for convenience.
	IniSection* pSection = ini.section("section.sectionone");
	pSection->deleteSetting("key1");
	
	if (!ini.retrieveSetting("section.sectionone", "key1").isNull())
	{
		gLog << "Failed to delete the variable.";
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool TestDeleteINISection::executeTest()
{
	Ini ini("ExampleINI", ExampleINIs::getExampleINI());
	
	if (ini.section("section.list") == NULL)
	{
		gLog << "Section doesn't exist already!";
		return false;
	}
	
	ini.deleteSection("section.list");
	
	if (ini.section("section.list") != NULL)
	{
		gLog << "Failed to delete the section.";
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool TestReadINIWithErrors::executeTest()
{
	Ini ini("ErrorINI", ExampleINIs::getExampleError1());
	
	const QStringList& errors = ini.errors();
	if (errors.isEmpty())
	{
		gLog << "No errors detected in a bad file";
		return false;
	}
	else
	{
		gLog << "Following errors were detected:";
		foreach(QString error, errors)
		{
			gLog << QString("\t%1").arg(error);
		}
		return true;
	}
}
