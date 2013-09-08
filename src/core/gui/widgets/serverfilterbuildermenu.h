//------------------------------------------------------------------------------
// serverfilterbuildermenu.h
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id33120EA3_7525_471D_B8AAE607B642CC00
#define id33120EA3_7525_471D_B8AAE607B642CC00

#include <QMenu>

class Server;
class ServerListFilterInfo;

class ServerFilterBuilderMenu : public QMenu
{
	Q_OBJECT

	public:
		ServerFilterBuilderMenu(const Server& server, const ServerListFilterInfo& filter,
			QWidget* parent = 0);
		~ServerFilterBuilderMenu();

		const ServerListFilterInfo& filter() const;

	private:
		class PrivData;

		PrivData* d;

		QAction* addAction(QMenu* menu, const QString& text, const char* slot);
		QAction* mkExcludeWadAction(QMenu* menu, const QString& wadName);
		QAction* mkIncludeWadAction(QMenu* menu, const QString& wadName);

	private slots:
		void excludeWadFromAction();
		void includeWadFromAction();
};

#endif
