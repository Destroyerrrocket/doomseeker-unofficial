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
#include <QVariant>

class IniSection;

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
         * @brief Creates NULL variable.
         */
        IniVariable();

		IniVariable(IniSection* pSection, const QString& key);
		IniVariable(const IniSection* pSection, const QString& key);

		bool			isNull() const { return pConstSection == NULL; }
		QString			valueString() const { return this->value().toString(); }

		const IniVariable &operator=(const QString &str);
		const IniVariable &operator=(const char* str) { return *this = QString(str); }
		const IniVariable &operator=(int i);
		const IniVariable &operator=(unsigned int i);
		const IniVariable &operator=(short i);
		const IniVariable &operator=(unsigned short i);
		const IniVariable &operator=(bool b);
		const IniVariable &operator=(float f);
		const IniVariable &operator=(const IniVariable &other);

		QString operator*() const { return value().toString(); }
		operator QString () const { return value().toString(); }

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

		void				setValue(const QVariant& value);
		QVariant			value() const;

	private:
		friend class TestReadINIVariable;
		friend class TestReadINIList;

		/**
		 * @brief For non-const operations. Might be NULL even if pConstSection
		 *        is not NULL.
		 */
        IniSection*			pSection;

        /**
         * @brief For const operations. If NULL then IniVariable object is
         *        invalid.
         */
        const IniSection*	pConstSection;

		/**
		 *	@brief The key name of this variable.
		 */
		QString				key;


};

typedef QHash<QString, IniVariable>					IniVariables;	// the first QString is the name
typedef QHash<QString, IniVariable>::iterator		IniVariablesIt;
typedef QHash<QString, IniVariable>::const_iterator	IniVariablesConstIt;

#endif
