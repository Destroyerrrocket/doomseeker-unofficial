//------------------------------------------------------------------------------
// inivariable.h
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
#ifndef __INIVARIABLE_H__
#define __INIVARIABLE_H__

#include "global.h"

#include <QHash>
#include <QString>

/**
 *	@brief INI variable representation.
 *
 *	Structure containing variable's value and comments. The name of the
 *	variable is not contained in the structure itself to prevent redundancy.
 */
class MAIN_EXPORT IniVariable
{
	public:
		/**
		 *	@brief Comment placed on the right side of the variable.
		 */
		QString			sideComment;

		/**
		 *	@brief Comment placed on top of the variable.
		 */
		QString			topComment;


		IniVariable() : null(false) {}
		IniVariable(const QString& value) : null(false) { *this = value; }
		IniVariable(const char* value) : null(false) { *this = QString(value); }
		IniVariable(int value) : null(false) { *this = value; }
		IniVariable(unsigned int value) : null(false) { *this = value; }
		IniVariable(short value) : null(false) { *this = value; }
		IniVariable(unsigned short value) : null(false) { *this = value; }
		IniVariable(bool value) : null(false) { *this = value; }
		IniVariable(float value) : null(false) { *this = value; }

		bool			isNull() const { return null; }
		QString			valueString() const { return this->value; }

		const IniVariable &operator=(const QString &str);
		const IniVariable &operator=(const char* str) { return *this = QString(str); }
		const IniVariable &operator=(int i);
		const IniVariable &operator=(unsigned int i);
		const IniVariable &operator=(short i);
		const IniVariable &operator=(unsigned short i);
		const IniVariable &operator=(bool b);
		const IniVariable &operator=(float f);
		const IniVariable &operator=(const IniVariable &other);

		// IniVariables can be used as pointers to QStrings as well
		const QString *operator->() const { return &value; }
		const QString &operator*() const { return value; }
		operator const QString &() const { return value; }

		/**
		*	Attempts to convert the QString value to a float.
		*/
		operator float() const;
		/**
		*	Attempts to convert the QString value to a integer.
		*/
		operator int() const;
		operator unsigned int() const;

		operator short() const;
		operator unsigned short() const;
		/**
		*	Convert QString value to boolean, if possible. It's done by converting
		*	to numValue() first, then to bool.
		*/
		operator bool() const;
		operator IniVariable&() { return *this; }

	protected:
		friend class Ini;
		friend class IniSection;
		friend class TestReadINIVariable;
		friend class TestReadINIList;

		static IniVariable	makeNull() { IniVariable nullVar; nullVar.null = true; return nullVar; }
		bool				null;

		/**
		 *	@brief The key name of this variable with lettercase preserved.
		 */
		QString			key;

		/**
		 *	@brief Value of the variable.
		 */
		QString			value;
};

typedef QHash<QString, IniVariable>					IniVariables;	// the first QString is the name
typedef QHash<QString, IniVariable>::iterator		IniVariablesIt;
typedef QHash<QString, IniVariable>::const_iterator	IniVariablesConstIt;

#endif
