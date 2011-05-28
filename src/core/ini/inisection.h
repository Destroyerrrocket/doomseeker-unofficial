//------------------------------------------------------------------------------
// inisection.h
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
#ifndef __INISECTION_H__
#define __INISECTION_H__

#include "inivariable.h"

#include <QVector>

/**
 *	@brief INI section representation.
 *
 *	Contains list of variables that are in this section, section's comments
 *	and namelists.
 */
class MAIN_EXPORT IniSection
{
	public:
		IniSection() : null(false) {}

		/**
		 *	@brief Comment placed on the right side of the section.
		 */
		QString					sideComment;

		/**
		 *	@brief Comment placed on top of the section.
		 */
		QString					topComment;

		/**
		 *	@brief List of strings that belong to this section.
		 *
		 *	This is an extension to the original INI format.
		 *	See Ini for more information.
		 */
		QVector<IniVariable>	nameList;

		static IniVariable nullVariable;

		IniVariable				&createSetting(const QString& name, const IniVariable& data);
		void					deleteSetting(const QString& name);
		bool					isNull() const { return null; }
		IniVariable				&retrieveSetting(const QString& name);
		const IniVariable		&retrieveSetting(const QString& name) const;
		const QString&			sectionName() const { return this->name; }
		IniVariable				&setting(const QString& name);

		IniVariable				&operator[](const QString& name) { return setting(name); }
		const IniVariable		&operator[](const QString& name) const { return retrieveSetting(name); }

	protected:
		friend class Ini;
		friend class TestReadINIList;

		static IniSection	makeNull() { IniSection nullSect; nullSect.null = true; return nullSect; }
		bool				null;

		/**
		 *	@brief A name of this section with lettercase preserved.
		 */
		QString					name;


		/**
		 *	@brief List of variables that belong to this section.
		 */
		IniVariables			variables;
};

typedef QHash<QString, IniSection> 					IniSections;	// the first QString is the name
typedef QHash<QString, IniSection>::iterator 		IniSectionsIt;
typedef QHash<QString, IniSection>::const_iterator 	IniSectionsConstIt;

#endif
