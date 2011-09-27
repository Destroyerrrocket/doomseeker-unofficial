//------------------------------------------------------------------------------
// oldinivariable.h
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
#ifndef __OLDINIVARIABLE_H__
#define __OLDINIVARIABLE_H__

#include "global.h"

#include <QHash>
#include <QString>

/**
 *	@brief OLDINI variable representation.
 *
 *	Structure contaoldining variable's value and comments. The name of the
 *	variable is not contained in the structure itself to prevent redundancy.
 */
class MAIN_EXPORT OldIniVariable
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


		OldIniVariable() : null(false) {}
		OldIniVariable(const QString& value) : null(false) { *this = value; }
		OldIniVariable(const char* value) : null(false) { *this = QString(value); }
		OldIniVariable(int value) : null(false) { *this = value; }
		OldIniVariable(unsigned int value) : null(false) { *this = value; }
		OldIniVariable(short value) : null(false) { *this = value; }
		OldIniVariable(unsigned short value) : null(false) { *this = value; }
		OldIniVariable(bool value) : null(false) { *this = value; }
		OldIniVariable(float value) : null(false) { *this = value; }

		bool			isNull() const { return null; }
		QString			valueString() const { return this->value; }

		const OldIniVariable &operator=(const QString &str);
		const OldIniVariable &operator=(const char* str) { return *this = QString(str); }
		const OldIniVariable &operator=(int i);
		const OldIniVariable &operator=(unsigned int i);
		const OldIniVariable &operator=(short i);
		const OldIniVariable &operator=(unsigned short i);
		const OldIniVariable &operator=(bool b);
		const OldIniVariable &operator=(float f);
		const OldIniVariable &operator=(const OldIniVariable &other);

		// OldIniVariables can be used as pointers to QStrings as well
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
		operator OldIniVariable&() { return *this; }

	protected:
		friend class OldIni;
		friend class OldIniSection;
		friend class TestReadOLDINIVariable;
		friend class TestReadOLDINIList;

		static OldIniVariable	makeNull() { OldIniVariable nullVar; nullVar.null = true; return nullVar; }
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

typedef QHash<QString, OldIniVariable>					OldIniVariables;	// the first QString is the name
typedef QHash<QString, OldIniVariable>::iterator		OldIniVariablesIt;
typedef QHash<QString, OldIniVariable>::const_iterator	OldIniVariablesConstIt;

#endif
