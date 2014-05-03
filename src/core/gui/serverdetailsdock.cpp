//------------------------------------------------------------------------------
// serverdetailsdock.cpp
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

#include <QBoxLayout>

#include "serverdetailsdock.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"
#include "serverapi/serverptr.h"
#include "serverapi/serverstructs.h"
#include "serverapi/tooltips/tooltipgenerator.h"

ServerDetailsDock::ServerDetailsDock(QWidget *parent) : QDockWidget(parent)
{
	setupUi(this);
	this->toggleViewAction()->setIcon(QIcon(":/icons/server_details.png"));

	clear();
}

ServerDetailsDock::~ServerDetailsDock()
{
}

void ServerDetailsDock::clear()
{
	lblServer->setText("");
	sbLabel->setText("");
	detailsLabel->setText("");
}

void ServerDetailsDock::displaySelection(QList<ServerPtr> &selectedServers)
{
	ServerPtr server = selectServer(selectedServers);
	if (server == NULL)
	{
		clear();
		return;
	}

	lblServer->setText(server->name());
	TooltipGenerator* tooltipGenerator = server->tooltipGenerator();
	if(server->players().numClients() != 0)
		sbLabel->setText(tooltipGenerator->playerTableHTML());
	detailsLabel->setText(tooltipGenerator->dmflagsHTML());
	delete tooltipGenerator;
}

void ServerDetailsDock::reorientContentsBasingOnDimensions()
{
	const int TOPTOBOTTOM_PREFERENCE_THRESHOLD = 100;
	if (height() + TOPTOBOTTOM_PREFERENCE_THRESHOLD > width())
	{
		static_cast<QBoxLayout*>(dataLayout)->setDirection(QBoxLayout::TopToBottom);
	}
	else
	{
		static_cast<QBoxLayout*>(dataLayout)->setDirection(QBoxLayout::LeftToRight);
	}
}

void ServerDetailsDock::resizeEvent(QResizeEvent *event)
{
	reorientContentsBasingOnDimensions();
}

ServerPtr ServerDetailsDock::selectServer(QList<ServerPtr> &selectedServers)
{
	if (selectedServers.count() == 0)
	{
		return ServerPtr();
	}
	if (!selectedServers[0]->isKnown())
	{
		return ServerPtr();
	}
	return selectedServers[0];
}

