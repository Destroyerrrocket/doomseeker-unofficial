//------------------------------------------------------------------------------
// dockserverinfo.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "gui/dockserverinfo.h"
#include <QLabel>

DockServerInfo::DockServerInfo(QWidget* parent) : QDockWidget(parent)
{
	setupUi(this);
	currentServer = NULL;
	updateServerInfo(NULL);

	dockWidgetContents->setHintSize(175, 200);

	mainLayoutDistanceFromRight = this->width() - mainLayout->width();
	mainLayoutDistanceFromBottom = this->height() - mainLayout->height();
}

void DockServerInfo::destroyServerInfo()
{
	QList<QWidget*>::iterator it;
	for (it = removalList.begin(); it != removalList.end(); ++it)
	{
		delete *it;
	}
	removalList.clear();
}

void DockServerInfo::resizeEvent(QResizeEvent* event)
{
	int w = this->width() - mainLayoutDistanceFromRight;
	int h = this->height() - mainLayoutDistanceFromBottom;
	mainLayout->resize(w, h);
}

void DockServerInfo::updateServerInfo(Server* server)
{
	if (server == currentServer)
		return;

	destroyServerInfo();
	static int x = 0;
	if (server == NULL)
	{
		currentServer = NULL;
	}
	else
	{
		currentServer = server;

		QList<ServerInfo>* infolist = server->serverInfo();
		QList<ServerInfo>::iterator it;
		for (it = infolist->begin(); it != infolist->end(); ++it)
		{
			QLabel* label = new QLabel(it->richText, this);
			label->setOpenExternalLinks(true);
			label->setToolTip(it->toolTip);
			label->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
			label->setWordWrap(true);

			infoLayout->addWidget(label);
			removalList.append(label);
		}

		delete infolist;
	}
}
