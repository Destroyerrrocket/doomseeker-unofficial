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

	connect(this, SIGNAL( dockLocationChanged(Qt::DockWidgetArea) ), SLOT( handleLocation(Qt::DockWidgetArea) ));
}

ServerDetailsDock::~ServerDetailsDock()
{
}

void ServerDetailsDock::displaySelection(QList<ServerPtr> &selectedServers)
{
	if(selectedServers.count() == 0)
	{
		sbLabel->setText("");
		detailsLabel->setText("");
		return;
	}

	ServerPtr selectedServer = selectedServers[0];

	TooltipGenerator* tooltipGenerator = selectedServer->tooltipGenerator();

	QString ret;
	if(selectedServer->players().numClients() != 0)
		ret = tooltipGenerator->playerTableHTML();
	sbLabel->setText(ret);

	delete tooltipGenerator;

	QString details;
	const QList<DMFlagsSection> sections = selectedServer->dmFlags();
	foreach(const DMFlagsSection &section, sections)
	{
		details += section.name() + ":\n";
		for(unsigned int i = 0;i < section.count();++i)
		{
			details += section[i].name() + '\n';
		}
		details += '\n';
	}

	detailsLabel->setText(details);
}

void ServerDetailsDock::handleLocation(Qt::DockWidgetArea area)
{
	switch(area)
	{
		case Qt::LeftDockWidgetArea:
		case Qt::RightDockWidgetArea:
			// Try to reorient the widgets if we're docking in a vertical position.
			// We can't assume the area value is good enough since having the central
			// widget be a dock means that the RightDockWidgetArea is favored.
			if(width() < height())
			{
				static_cast<QBoxLayout*>(widget()->layout())->setDirection(QBoxLayout::TopToBottom);
				break;
			}
		default:
			static_cast<QBoxLayout*>(widget()->layout())->setDirection(QBoxLayout::LeftToRight);
			break;
	}
}
