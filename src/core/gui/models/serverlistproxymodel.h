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

#include <QModelIndex>
#include <QSortFilterProxyModel>

class ServerListFilterInfo;
class ServerListHandler;
class Server;

class ServerListProxyModel : public QSortFilterProxyModel
{
	public:
		ServerListProxyModel(ServerListHandler* serverListHandler);
		~ServerListProxyModel();
		
		/**
		 *	@brief Filter info for edit.
		 *
		 *	@b Note: Do not forget to call invalidate() after edit is 
		 *	finished.
		 */
		ServerListFilterInfo*	filterInfo()
		{
			return pFilterInfo;
		}
		
		/**
		 *	@brief Sets new filter info and immediately calls invalidate()
		 */
		void					setFilterInfo(const ServerListFilterInfo& filterInfo);
		
		void					sortServers(int column, Qt::SortOrder order = Qt::AscendingOrder)
		{
			sortOrder = order;
			sort(column, order);
		}		

	protected:
		/**
		 *	Overloaded method that will filter out rows basing on pFilterInfo.
		 */
		bool					filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
		
	private:
		ServerListHandler*		parentHandler;
		
		/// Never NULL
		ServerListFilterInfo*	pFilterInfo;

		Qt::SortOrder 			sortOrder;
		
		bool					compareColumnSortData(QVariant& var1, QVariant& var2, int column) const;
		bool					lessThan(const QModelIndex& left, const QModelIndex& right) const;

		Server* 				serverFromList(const QModelIndex& index) const;
		Server*					serverFromList(int row) const;
};

#endif
