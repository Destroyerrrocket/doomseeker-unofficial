//------------------------------------------------------------------------------
// serverlistproxymodel.h
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
#ifndef __SERVERLISTPROXYMODEL_H__
#define __SERVERLISTPROXYMODEL_H__

#include "serverapi/serverptr.h"
#include <QModelIndex>
#include <QSortFilterProxyModel>

class ServerListFilterInfo;
class ServerListHandler;
class Server;


class ColumnSort
{
	public:
		static ColumnSort deserializeQVariant(const QVariant &v);

		ColumnSort();
		ColumnSort(int columnId, Qt::SortOrder order);

		int columnId() const;
		bool isValid() const;
		Qt::SortOrder order() const;

		bool operator==(const ColumnSort &other) const;

		QVariant serializeQVariant() const;

	private:
		int columnId_;
		Qt::SortOrder order_;
};


class ServerListProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	public:
		ServerListProxyModel(ServerListHandler* serverListHandler);
		~ServerListProxyModel();

		void addAdditionalColumnSorting(int column, Qt::SortOrder order);
		const QList<ColumnSort> &additionalSortColumns() const;
		void clearAdditionalSorting();
		const ServerListFilterInfo& filterInfo() const;

		bool isAnyColumnSortedAdditionally() const;
		bool isSortingAdditionallyByColumn(int column) const;

		void removeAdditionalColumnSorting(int column);
		void setAdditionalSortColumns(const QList<ColumnSort> &columns);
		/**
		 * @brief Sets new filter info and immediately calls invalidate()
		 */
		void setFilterInfo(const ServerListFilterInfo& filterInfo);
		void setGroupServersWithPlayersAtTop(bool b);

		void sortServers(int column, Qt::SortOrder order = Qt::AscendingOrder);

	signals:
		void additionalSortColumnsChanged();

	protected:
		/**
		 * Overloaded method that will filter out rows basing on pFilterInfo.
		 */
		bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;

	private:
		class PrivData;

		PrivData* d;

		int compareColumnSortData(QVariant& var1, QVariant& var2, int column) const;
		bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

		ServerPtr serverFromList(const QModelIndex& index) const;
		ServerPtr serverFromList(int row) const;
};

#endif
