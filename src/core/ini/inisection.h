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

#include <QVariant>
#include <QVector>
#include "global.h"

class Ini;
class IniVariable;

/**
 * @brief INI section representation.
 *
 * Contains list of variables that are in this section.
 */
class MAIN_EXPORT IniSection
{
	public:
        /**
         * @brief Creates invalid IniSection object.
         */
        IniSection();

        /**
         *
         */
		IniSection(Ini* pIni, const QString& sectionName);

		IniVariable				createSetting(const QString& name, const QVariant& data);
		void					deleteSetting(const QString& name);
		bool					isNull() const { return d.pIni == NULL; }
		IniVariable				retrieveSetting(const QString& name);
		const IniVariable		retrieveSetting(const QString& name) const;
		const QString&			sectionName() const { return d.name; }
		IniVariable				setting(const QString& name);
		void 					setValue(const QString& key, const QVariant& value);

		IniVariable				operator[](const QString& name);
		const IniVariable		operator[](const QString& name) const;

		QVariant 				value(const QString& key) const;

	private:
        class PrivData
        {
            public:
                /**
                 * @brief A name of this section with lettercase preserved.
                 */
                QString					name;

                /**
                 * @brief Ini file to which this section belongs to.
                 */
                Ini*                    pIni;
        };

        PrivData d;

        void remove(const QString& key);
};

typedef QHash<QString, IniSection> 					IniSections;	/// the first QString is the name
typedef QHash<QString, IniSection>::iterator 		IniSectionsIt;
typedef QHash<QString, IniSection>::const_iterator 	IniSectionsConstIt;

#endif
