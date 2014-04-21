//------------------------------------------------------------------------------
// serverdetailsdock.h
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
// Copyright (C) 2014 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __SERVERDETAILSDOCK_H__
#define __SERVERDETAILSDOCK_H__

#include <QDockWidget>

#include "serverapi/serverptr.h"

#include "ui_serverdetailsdock.h"

class ServerDetailsDock : public QDockWidget, private Ui::ServerDetailsDock
{
	Q_OBJECT

public:
	ServerDetailsDock(QWidget *parent);
	~ServerDetailsDock();

public slots:
	void displaySelection(QList<ServerPtr>&);

protected slots:
	void handleLocation(Qt::DockWidgetArea);

private:
	void clear();
	ServerPtr selectServer(QList<ServerPtr>&);
};

#endif
