//------------------------------------------------------------------------------
// ini.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ini.h"
#include "log.h"
#include "main.h"
#include "strings.h"
#include <cstdio>
#include <cstdlib>

bool IniVariable::boolValue() const
{
	return numValue() != 0;
}

int IniVariable::numValue() const
{
	return value.toInt();
}

unsigned IniVariable::numUnsignedValue() const
{
	return value.toUInt();
}

float IniVariable::numValueFloat() const
{
	return value.toFloat();
}

void IniVariable::setValue(int i)
{
	char buf[32];
	sprintf(buf, "%d", i);
	value = buf;
}

void IniVariable::setValue(unsigned int i)
{
	char buf[32];
	sprintf(buf, "%u", i);
	value = buf;
}

void IniVariable::setValue(bool b)
{
	setValue(static_cast<int>(b));
}

void IniVariable::setValue(float f)
{
	char buf[32];
	sprintf(buf, "%f", f);
	value = buf;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IniVariable* IniSection::createSetting(const QString& name, const IniVariable& data)
{
	if (name.isEmpty())
	{
		return NULL;
	}
	
	QString nameLower = name.toLower();

	IniVariablesIt it = variables.find(nameLower);
	if (it != variables.end())
	{
		return &it->second;
	}

	variables.insert(IniVariablesPair(nameLower, data));
	IniVariable* pNewVariable = &variables.find(nameLower)->second;
	pNewVariable->key = name;
	
	return pNewVariable;
}

void IniSection::deleteSetting(const QString& name)
{
	if (name.isEmpty())
	{
		return;
	}
	
	QString nameLower = name.toLower();

	IniVariablesIt it = variables.find(nameLower);
	if (it != variables.end())
	{
		variables.erase(it);
	}
}

IniVariable* IniSection::retrieveSetting(const QString& name)
{
	if (name.isEmpty())
	{
		return NULL;
	}
	
	QString nameLower = name.toLower();

	IniVariablesIt it = variables.find(nameLower);
	if (it == variables.end())
	{
		return NULL;
	}

	return &it->second;
}

IniVariable* IniSection::setting(const QString& name)
{
	if (name.isEmpty())
	{
		return NULL;
	}
	
	QString nameLower = name.toLower();

	IniVariable* pVariable = retrieveSetting(nameLower);
	if (pVariable == NULL)
	{
		pVariable = createSetting(name, IniVariable());
	}

	return pVariable;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Ini::Ini(const QString& filename)
: dataSourc(Memory), valid(false)
{
	loadIniFile(filename);
}

Ini::Ini(const QString& displayName, const QByteArray& memorydata)
{
	filename = displayName;
	valid = loadAdditionalSettings(memorydata);
	dataSourc = Memory;
}

void Ini::copy(const Ini& other)
{
	foreach(QString str, other.errorsList)
	{
		errorsList << str;
	}
	
	dataSourc = other.dataSourc;
	filename = other.filename;
	iniTopComment = other.iniTopComment;
	sections = other.sections;
	valid = other.valid;
}

IniSection* Ini::createSection(const QString& name)
{
	if (name.isEmpty())
	{
		return NULL;
	}
	
	QString nameLower = name.toLower();

	IniSectionsIt it = sections.find(nameLower);
	if (it != sections.end())
	{
		return &it->second;
	}
	
	IniSection newSection;
	newSection.name = name;

	sections.insert(IniSectionsPair(nameLower, newSection));
	return &sections.find(nameLower)->second;
}

IniVariable* Ini::createSetting(const QString& sectionName, const QString& name, const IniVariable& data)
{
	IniSection* pSection = createSection(sectionName);
	if (pSection == NULL)
	{
		return NULL;
	}

	return pSection->createSetting(name, data);
}

void Ini::deleteSection(const QString& sectionName)
{
	QString nameLower = sectionName.toLower();

	IniSectionsIt it = sections.find(nameLower);
	if (it != sections.end())
	{
		// Since there are no pointers stored on the list we can just drop
		// entire section.
		sections.erase(it);
	}
}

void Ini::deleteSetting(const QString& sectionName, const QString& settingName)
{
	IniSection* pSection = section(sectionName);
	if (pSection != NULL)
	{
		pSection->deleteSetting(settingName);
	}
}

QByteArray Ini::iniData()
{
	QByteArray output;
	structuresIntoQByteArray(output);
	return output;
}

bool Ini::loadAdditionalSettings(const QString& filename)
{
	QFile file(filename);
	if (!file.exists())
	{
		errorsList << tr("Fatal error: file %1 doesn't exist!").arg(filename);
		return false;
	}

	unsigned int size = file.size();
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	if (!valid)
	{
		dataSourc = Memory;
	}

	valid = true;
	QByteArray fileContent = file.read(size);
	readQByteArrayIntoStructures(fileContent);
	return true;
}

bool Ini::loadAdditionalSettings(const QByteArray& data)
{
	readQByteArrayIntoStructures(data);
	return true;
}

void Ini::loadIniFile(const QString& fileName)
{
	QString configDirPath = Main::dataPaths->programsDataDirectoryPath();

	if (configDirPath.isEmpty())
	{
		gLog << tr("Could not get an access to the settings directory. Configuration will not be saved.");
		return;
	}

	filename = configDirPath + "/" + fileName;
	gLog << tr("Ini file is: %1").arg(filename);

	QFile file(filename);
	if (!file.exists())
	{
		if (!file.open(QIODevice::WriteOnly))
		{
			errorsList << tr("Fatal error: file %1 doesn't exist and cannot be created!").arg(filename);
			valid = false;
			return;
		}
	}
	file.close();

	valid = loadAdditionalSettings(filename);
	if (valid)
	{
		dataSourc = Drive;
	}
}

Ini& Ini::operator=(const Ini& other)
{
	if (this != &other)
	{
		copy(other);
	}

	return *this;
}

IniSection* Ini::parseSectionName(QString& line, bool& ok, const QString& topComment, unsigned lineNum)
{
	QString 			sectionName;
	QString 			sideComment;

	ok = true;

	line = Strings::triml(line, " \t[");

	int n = line.indexOf("]", 0);

	if (n < 0)
	{
		errorsList << tr("Error in file '%1' at line %2: ] expected but not found").arg(filename).arg(lineNum);
		ok = false;
		return NULL;
	}

	sectionName = line.mid(0, n);
	sectionName = Strings::trimr(sectionName, " \t]");

	++n;
	line = line.mid(n, line.size() - n);
	line = Strings::trim(line, " \t");
	if (line.size() > 0 && line[0] == '#')
	{
		sideComment = line.mid(1);
	}

	IniSection section;
	section.topComment = topComment;
	section.sideComment = sideComment;
	section.name = sectionName;
	
	QString sectionNameLower = sectionName.toLower();	
	
	IniSectionsIt sectionIt = sections.find(sectionNameLower);

	if ( sectionIt != sections.end())
	{
		sectionIt->second.topComment += "\n";
		sectionIt->second.topComment += topComment;
		return &sectionIt->second;
	}
	else
	{
		sections.insert(IniSectionsPair(sectionNameLower, section));
		return &sections[sectionNameLower];
	}
}

void Ini::print() const
{
	QByteArray ini;
	structuresIntoQByteArray(ini);

	printf("%s\n", QString(ini).toAscii().constData());
}

void Ini::readQByteArrayIntoStructures(const QByteArray& array)
{
	unsigned		lineNum = 0;

	IniSection*		currentSection = NULL;
	bool 			previousLineWasEmpty = false;
	QString			topComment;
	IniVariables 	vars;
	
	QList<QByteArray> arrayLines = array.split('\n');

	foreach(QByteArray arrayLine, arrayLines)
	{
		++lineNum;
		
		QString	sideComment;			
		QString line = arrayLine;
		
		line = Strings::trimr(line, " \t\r");
		line = Strings::triml(line, " \t");

		if (line.isEmpty() && previousLineWasEmpty && currentSection == NULL) // top comment of the entire file
		{
			previousLineWasEmpty = false;
			iniTopComment += topComment;
			topComment.clear();
		}
		else if (line.isEmpty())
		{
			previousLineWasEmpty = true;
			continue;
		}
		else if (line[0] == '#') // comment
		{
			previousLineWasEmpty = false;
			topComment += line.mid(1) + '\n';
		}
		else if (line[0] == '[') // section
		{
			previousLineWasEmpty = false;
			bool ok;
			
			topComment = Strings::trimr(topComment, "\n");
			
			currentSection = parseSectionName(line, ok, topComment, lineNum);
			topComment.clear();
			if (!ok)
			{
				valid = false;
				return;
			}
		}
		else // something else, perhaps a variable
		{
			previousLineWasEmpty = false;
			if (currentSection == NULL)
			{
				errorsList << tr("Warning in file '%1' at line %2: found data that doesn't belong to any section.").arg(filename).arg(lineNum);
			}

			int equal = line.indexOf("=", 0);

			QString varValue;
			QString varName;

			if (equal >= 0)
			{
				varName = line.mid(0, equal);
				varName = Strings::trimr(varName, " \t");
				if (varName.isEmpty())
				{
					errorsList << tr("Error in file '%1' at line %2: found variable without a name. Ignoring").arg(filename).arg(lineNum);
					continue;
				}
			}

			line = line.mid(equal + 1, line.size() - (equal + 1));
			line = Strings::triml(line, " \t");
			if (line[0] == '\"') // We seek the next occurence of "
			{
				int secondQuote = line.indexOf("\"", 1);
				if (secondQuote < 0)
				{
					errorsList << tr("Error in file '%1' at line %2: found opening '\"' but no closing one.").arg(filename).arg(lineNum);
					valid = false;
					return;
				}

				varValue = line.mid(1, secondQuote - 1);
				line = line.mid(secondQuote + 1, line.size() - secondQuote - 1);
			}
			else
			{
				int firstHash = line.indexOf("#", 1);

				int firstSpace = line.indexOf(" ", 1);
				if (firstSpace < 0 || (firstHash > 0 && firstSpace > firstHash))
				{
					firstSpace = line.indexOf("\t", 1);
					if (firstSpace < 0 || (firstHash > 0 && firstSpace > firstHash))
					{
						firstSpace = firstHash > 0 ? firstHash : line.size();
					}
				}

				varValue = line.mid(0, firstSpace);
				line = line.mid(firstSpace, line.size() - firstSpace);
			}

			line = Strings::triml(line, " \t");
			if (line.size() > 0)
			{
				if (line[0] == '#')
				{
					sideComment = line.mid(1);
				}
				else
				{
					errorsList << tr("Warning in file '%1' at line %2: Unknown data after variable's value. Ignoring.").arg(filename).arg(lineNum);				
				}
			}

			IniVariable var;
			var.topComment = topComment;
			var.sideComment = sideComment;
			var.key = varName;
			var.value = varValue;
			
			var.topComment = Strings::trimr(var.topComment, "\n");

			topComment.clear();

			QString varNameLower = varName.toLower();
			if (!varNameLower.isEmpty())
			{
				IniVariablesIt variablesIt = currentSection->variables.find(varNameLower);

				if ( variablesIt != currentSection->variables.end())
				{
					// Replace variable with new one
					variablesIt->second = var;
				}
				else
				{
					// Add variable
					currentSection->variables.insert(IniVariablesPair(varNameLower, var));
				}
			}
			else
			{
				// This is a name list.
				currentSection->nameList.push_back(var);
			}
		}
	}
	
	iniTopComment = Strings::trimr(iniTopComment, "\n");
}

IniVariable* Ini::retrieveSetting(const QString& sectionName, const QString& variableName)
{
	if (sectionName.isEmpty() || variableName.isEmpty())
	{
		return NULL;
	}

	IniSection* pSection = section(sectionName);
	if (pSection == NULL)
	{
		return NULL;
	}

	return pSection->retrieveSetting(variableName);
}

bool Ini::save()
{
	if (dataSourc == Memory)
	{
		return false;
	}

	QByteArray output;
	structuresIntoQByteArray(output);

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
	{
		return false;
	}

	if (!file.write(output, output.size()))
	{
		return false;
	}

	return true;
}

IniSection* Ini::section(const QString& name)
{
	if (name.isEmpty())
	{
		return NULL;
	}
	
	QString nameLower = name.toLower();

	IniSectionsIt it = sections.find(nameLower);
	if (it == sections.end())
	{
		return NULL;
	}

	return &it->second;
}

IniVariable* Ini::setting(const QString& sectionName, const QString& variableName)
{
	if (sectionName.isEmpty() || variableName.isEmpty())
	{
		return NULL;
	}

	IniVariable* var = retrieveSetting(sectionName, variableName);
	if (var == NULL)
	{
		var = createSetting(sectionName, variableName, IniVariable());
	}

	return var;
}

void Ini::structuresIntoQByteArray(QByteArray& output) const
{
	output.clear();
	IniSectionsConstIt sectionit;
	if (!iniTopComment.isEmpty())
	{
		QStringList commentList;
		commentList = iniTopComment.split('\n');

		foreach (QString comment, commentList)
		{
			output.append("# ");
			output.append(comment);
			output.append("\n");
		}
		output.append("\n");
	}

	for (sectionit = sections.begin(); sectionit != sections.end(); ++sectionit)
	{
		const IniSection& section = sectionit->second;
		QString sectionTopComment = section.topComment;
		
		sectionTopComment = Strings::trim(sectionTopComment, "\r\n");
	
		// Output section's top comment
		if (!sectionTopComment.isEmpty())
		{
			QStringList commentList;
			commentList = sectionTopComment.split('\n');
			
			foreach (QString comment, commentList)
			{
				output.append("# ");
				output.append(comment);
				output.append("\n");
			}
		}
		
		// Output section's name
		output.append("\n[ ");
		output.append(section.name);
		output.append(" ]");
		
		QString sectionSideComment = section.sideComment;
		sectionSideComment = sectionSideComment.replace("\n", " ");
		if (!sectionSideComment.isEmpty())
		{
			// Output section's side comment
			output.append("\t# ");
			output.append(sectionSideComment);
		}
		output.append("\n");

		IniVariablesConstIt varit;
		for (varit = sectionit->second.variables.begin(); varit != sectionit->second.variables.end(); ++varit)
		{
			const IniVariable& variable = varit->second;
		
			// Output variable's top comment
			QString variableTopComment = variable.topComment;
			variableTopComment = Strings::trim(variableTopComment, "\r\n");
			
			if (!variableTopComment.isEmpty())
			{
				QStringList commentList = variableTopComment.split('\n');
				
				foreach (QString comment, commentList)
				{
					output.append("# ");
					output.append(comment);
					output.append("\n");
				}
			}

			// Output variable's name and value, make sure to wrap values
			// with '#' or whitespace characters wrapped in quotation marks.
			QString formattedValue = variable.value;
			
			QRegExp whiteSpace("\\s");
			
			if (variable.value.contains(whiteSpace) || variable.value.contains("#"))
			{
				formattedValue = QString("\"%1\"").arg(formattedValue);
			}
			
			output.append(variable.key);
			output.append(" = ");
			output.append(formattedValue);

			// Output variable's side comment
			QString variableSideComment = varit->second.sideComment;
			variableSideComment = variableSideComment.replace("\n", " ");
			
			if (!variableSideComment.isEmpty())
			{
				output.append("\t#");
				output.append(variableSideComment);
			}

			output.append("\n", 1);
		}

		QVector<IniVariable>::const_iterator nameit;
		for (nameit = sectionit->second.nameList.begin(); nameit != sectionit->second.nameList.end(); ++nameit)
		{
			QString nameListTopComment = nameit->topComment;
			nameListTopComment = Strings::trim(nameListTopComment, "\r\n");
		
			if (!nameListTopComment.isEmpty())
			{
				QStringList commentList = nameListTopComment.split('\n');
				foreach (QString comment, commentList)
				{
					output.append("# ");
					output.append(comment);
					output.append("\n");
				}
			}

			// Output variable's name and value
			QString formattedValue = nameit->value;
			
			QRegExp whiteSpace("\\s");
			
			if (nameit->value.contains(whiteSpace) || nameit->value.contains("#"))
			{
				formattedValue = QString("\"%1\"").arg(formattedValue);
			}
			
			output.append(formattedValue);
			
			// Output variable's side comment
			QString nameListSideComment = nameit->sideComment;
			nameListSideComment = nameListSideComment.replace("\n", " ");

			if (!nameListSideComment.isEmpty())
			{
				output.append("\t#");
				output.append(nameit->sideComment);
			}

			output.append("\n");
		}

		output.append("\n");
	}
}

