//------------------------------------------------------------------------------
// oldini.cpp
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
#include "oldini.h"
#include "log.h"
#include "main.h"
#include "strings.h"

#include <cassert>

OldIniSection OldIni::nullSection = OldIniSection::makeNull();

OldIni::OldIni(const QString& filename)
: dataSourc(Memory), firstRun(false), valid(false)
{
	loadOldIniFile(filename);
}

OldIni::OldIni(const QString& displayName, const QByteArray& memorydata) : firstRun(false)
{
	filename = displayName;
	valid = loadAdditionalSettings(memorydata);
	dataSourc = Memory;
}

void OldIni::copy(const OldIni& other)
{
	foreach(QString str, other.errorsList)
	{
		errorsList << str;
	}

	dataSourc = other.dataSourc;
	filename = other.filename;
	oldiniTopComment = other.oldiniTopComment;
	sections = other.sections;
	firstRun = other.firstRun;
	valid = other.valid;
}

OldIniSection& OldIni::createSection(const QString& name)
{
	if (name.isEmpty())
	{
		return nullSection;
	}

	QString nameLower = name.toLower();

	OldIniSectionsIt it = sections.find(nameLower);
	if (it != sections.end())
	{
		return *it;
	}

	OldIniSection newSection;
	newSection.name = name;

	sections.insert(nameLower, newSection);
	return *sections.find(nameLower);
}

OldIniVariable &OldIni::createSetting(const QString& sectionName, const QString& name, const OldIniVariable& data)
{
	OldIniSection& pSection = createSection(sectionName);
	if (pSection.isNull())
	{
		return OldIniSection::nullVariable;
	}

	return pSection.createSetting(name, data);
}

void OldIni::deleteSection(const QString& sectionName)
{
	QString nameLower = sectionName.toLower();

	OldIniSectionsIt it = sections.find(nameLower);
	if (it != sections.end())
	{
		// Since there are no pointers stored on the list we can just drop
		// entire section.
		sections.erase(it);
	}
}

void OldIni::deleteSetting(const QString& sectionName, const QString& settingName)
{
	OldIniSection& pSection = section(sectionName);
	if (!pSection.isNull())
	{
		pSection.deleteSetting(settingName);
	}
}

QByteArray OldIni::oldiniData()
{
	QByteArray output;
	structuresIntoQByteArray(output);
	return output;
}

bool OldIni::loadAdditionalSettings(const QString& filename)
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

bool OldIni::loadAdditionalSettings(const QByteArray& data)
{
	readQByteArrayIntoStructures(data);
	return true;
}

bool OldIni::loadOldIniFile(const QString& filePath)
{
	gLog << tr("OldIni file is: %1").arg(filePath);

	this->filename = filePath;

	QFile file(filePath);
	if (!file.exists())
	{
		firstRun = true;
		if (!file.open(QIODevice::WriteOnly))
		{
			errorsList << tr("Fatal error: file %1 doesn't exist and cannot be created!").arg(filePath);
			valid = false;
			return false;
		}
	}
	file.close();

	valid = loadAdditionalSettings(filePath);
	if (valid)
	{
		dataSourc = Drive;
	}

	return valid;
}

OldIni& OldIni::operator=(const OldIni& other)
{
	if (this != &other)
	{
		copy(other);
	}

	return *this;
}

OldIniSection& OldIni::parseSectionName(QString& line, bool& ok, const QString& topComment, unsigned lineNum)
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
		return nullSection;
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

	OldIniSection &section = this->section(sectionName);
	if(section.isNull())
	{
		OldIniSection &newSection = createSection(sectionName);
		newSection.topComment = topComment;
		newSection.sideComment = sideComment;
		newSection.name = sectionName;
		return newSection;
	}
	section.topComment = topComment;
	section.sideComment = sideComment;
	section.name = sectionName;

	return section;
}

void OldIni::print() const
{
	QByteArray oldini;
	structuresIntoQByteArray(oldini);

	printf("%s\n", QString(oldini).toAscii().constData());
}

void OldIni::readQByteArrayIntoStructures(const QByteArray& array)
{
	assert(OldIniSection::nullVariable.isNull());
	assert(nullSection.isNull());

	unsigned		lineNum = 0;

	OldIniSection*		currentSection = &nullSection;
	bool 			previousLineWasEmpty = false;
	QString			topComment;
	OldIniVariables 	vars;

	QList<QByteArray> arrayLines = array.split('\n');

	foreach(QByteArray arrayLine, arrayLines)
	{
		++lineNum;

		QString	sideComment;
		QString line = arrayLine;

		line = Strings::trimr(line, " \t\r");
		line = Strings::triml(line, " \t");

		if (line.isEmpty() && previousLineWasEmpty && currentSection->isNull()) // top comment of the entire file
		{
			previousLineWasEmpty = false;
			oldiniTopComment += topComment;
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

			currentSection = &parseSectionName(line, ok, topComment, lineNum);
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
			if (currentSection->isNull())
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

			if(!varName.isEmpty() && !currentSection->isNull())
			{
				OldIniVariable &var = currentSection->setting(varName);
				assert(!var.isNull());
				var.topComment = topComment;
				var.sideComment = sideComment;
				var.key = varName;
				var.value = varValue;

				var.topComment = Strings::trimr(var.topComment, "\n");
			}
			else
			{
				// String list
				OldIniVariable var;
				var.topComment = topComment;
				var.sideComment = sideComment;
				var.key = varName;
				var.value = varValue;
				currentSection->nameList.push_back(var);
			}

			topComment.clear();
		}
	}

	oldiniTopComment = Strings::trimr(oldiniTopComment, "\n");
}

OldIniSection& OldIni::retrieveSection(const QString& name)
{
	if (name.isEmpty())
	{
		return nullSection;
	}

	QString nameLower = name.toLower();

	OldIniSectionsIt it = sections.find(nameLower);
	if (it == sections.end())
	{
		return nullSection;
	}

	return *it;
}

OldIniVariable &OldIni::retrieveSetting(const QString& sectionName, const QString& variableName)
{
	if (sectionName.isEmpty() || variableName.isEmpty())
	{
		return OldIniSection::nullVariable;
	}

	OldIniSection& pSection = section(sectionName);
	if (pSection.isNull())
	{
		return OldIniSection::nullVariable;
	}

	return pSection.retrieveSetting(variableName);
}

bool OldIni::save()
{
	if (dataSourc == Memory)
	{
		return false;
	}

	gLog << tr("Saving oldini file: \"%1\"").arg(filename);

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

OldIniSection& OldIni::section(const QString& name)
{
	return createSection(name);
}

QVector<OldIniSection*> OldIni::sectionsArray(const QString& regexPattern)
{
	QVector<OldIniSection*> sectionsReferencesArray;

	QRegExp regExp(regexPattern, Qt::CaseInsensitive);

	OldIniSectionsIt it;
	for (it = sections.begin(); it != sections.end(); ++it)
	{
		const QString& key = it.key();
		if (key.contains(regExp))
		{
			sectionsReferencesArray << &it.value();
		}
	}

	return sectionsReferencesArray;
}

OldIniVariable& OldIni::setting(const QString& sectionName, const QString& variableName)
{
	if (sectionName.isEmpty() || variableName.isEmpty())
	{
		return OldIniSection::nullVariable;
	}

	OldIniVariable &var = retrieveSetting(sectionName, variableName);
	if (var.isNull())
	{
		return createSetting(sectionName, variableName, OldIniVariable());
	}

	return var;
}

void OldIni::structuresIntoQByteArray(QByteArray& output) const
{
	output.clear();
	OldIniSectionsConstIt sectionit;
	if (!oldiniTopComment.isEmpty())
	{
		QStringList commentList;
		commentList = oldiniTopComment.split('\n');

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
		const OldIniSection& section = *sectionit;
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

		OldIniVariablesConstIt varit;
		for (varit = sectionit->variables.begin(); varit != sectionit->variables.end(); ++varit)
		{
			const OldIniVariable& variable = *varit;

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
			QString variableSideComment = varit->sideComment;
			variableSideComment = variableSideComment.replace("\n", " ");

			if (!variableSideComment.isEmpty())
			{
				output.append("\t#");
				output.append(variableSideComment);
			}

			output.append("\n", 1);
		}

		QVector<OldIniVariable>::const_iterator nameit;
		for (nameit = sectionit->nameList.begin(); nameit != sectionit->nameList.end(); ++nameit)
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
