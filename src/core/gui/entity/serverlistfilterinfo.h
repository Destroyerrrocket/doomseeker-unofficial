//------------------------------------------------------------------------------
// serverlistfilterinfo.h
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __SERVERLISTFILTERINFO_H__
#define __SERVERLISTFILTERINFO_H__

#include <QString>
#include <QStringList>

/**
 *	@brief Structure describing server filter.
 */
class ServerListFilterInfo
{
public:
	bool bEnabled;
	bool bShowEmpty;
	bool bShowFull;
	bool bShowOnlyValid;
	QStringList gameModes;
	QStringList gameModesExcluded;

	/**
	 *	@brief Maximum allowed ping.
	 *
	 *	The default value is 0 and it means that ping filter
	 *	is disabled.
	 */
	unsigned maxPing;
	QString serverName;
	QStringList wads;
	QStringList wadsExcluded;

	ServerListFilterInfo();
	ServerListFilterInfo(const ServerListFilterInfo& other)
	{
		copy(other);
	}

	ServerListFilterInfo& operator=(const ServerListFilterInfo& other)
	{
		if (this != &other)
		{
			copy(other);
		}

		return *this;
	}

	/**
	 * @brief Informs if filter will apply to any server.
	 *
	 * @return true is returned if this filter will cause servers to become
	 * hidden from the server table, false if filter will never apply to any
	 * server.
	 */
	bool isFilteringAnything() const;

	QString toString() const;

private:
	void copy(const ServerListFilterInfo& other);
	void copyTrimmed(QStringList& target, const QStringList& source) const;
};

#endif
