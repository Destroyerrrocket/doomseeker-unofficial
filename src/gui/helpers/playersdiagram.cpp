// playerdiagram.cpp
//------------------------------------------------------------------------------
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include "playersdiagram.h"
#include "serverapi/server.h"
#include <QPainter>
#include <cassert>

const char* PlayersDiagram::slotStyles[NUM_SLOTSTYLES] = { "marines", "blocks" };
const QImage* PlayersDiagram::openImage = NULL;
const QImage* PlayersDiagram::openSpecImage = NULL;
const QImage* PlayersDiagram::botImage = NULL;
const QImage* PlayersDiagram::playerImage = NULL;
const QImage* PlayersDiagram::spectatorImage = NULL;

PlayersDiagram::PlayersDiagram(const Server *server)
: server(server), tmp(NULL)
{
	if(openImage == NULL)
	{
		return;
	}

	QPixmap diagram(server->maximumClients()*playerImage->width(), playerImage->height());
	diagram.fill(Qt::transparent);

	int slotSize = playerImage->width();
	int position = diagram.width()-slotSize;
	QPainter p(&diagram);

	// numSpectators is actually greater than numPlayers.  This is done
	// in order to simplify the drawing code.
	int numSpectators = server->numPlayers();
	int numPlayers = numSpectators;
	int numBots = numSpectators; // Bots not on a team
	int numPlayersTeam[MAX_TEAMS] = {0, 0, 0, 0};
	int numBotsTeam[MAX_TEAMS] = {0, 0, 0, 0};
	for(int i = 0; i < server->numPlayers(); ++i)
	{
		if(server->player(i).isSpectating())
		{
			numPlayers--;
			numBots--;
		}
		else if(server->player(i).teamNum() != Player::TEAM_NONE)
		{
			if(!server->player(i).isBot())
				numPlayersTeam[server->player(i).teamNum()]++;
			numBotsTeam[server->player(i).teamNum()]++;
		}
		else if(server->player(i).isBot())
		{
			numPlayers--;
		}
	}

	// Draw them
	int currentTeam = 0;
	int count = 0;
	for(unsigned short i = 0;i < server->maximumClients();i++)
	{
		const QImage *slot = openSpecImage;
		if(i < numPlayers)
		{
			while(numBotsTeam[currentTeam] == count && currentTeam < MAX_TEAMS)
			{
				count = 0;
				currentTeam++;
			}
			if(currentTeam >= MAX_TEAMS)
				currentTeam = Player::TEAM_NONE;

			if(currentTeam == Player::TEAM_NONE || count < numPlayersTeam[currentTeam])
				slot = colorizePlayer(playerImage, QColor(server->teamColor(currentTeam)));
			else
				slot = colorizePlayer(botImage, QColor(server->teamColor(currentTeam)));
			count++;
		}
		else if(i < numBots)
			slot = colorizePlayer(botImage, QColor(server->teamColor(Player::TEAM_NONE)));
		else if(i < numSpectators)
			slot = spectatorImage;
		else if(i < server->maximumPlayers())
			slot = openImage;
		assert(slot != NULL);
		p.drawImage(position, 0, *slot);
		position -= slotSize;
	}

	this->diagram = diagram;
}

PlayersDiagram::~PlayersDiagram()
{
	if(tmp != NULL)
		delete tmp;
}

const QImage* PlayersDiagram::colorizePlayer(const QImage *image, const QColor &color)
{
	if(tmp != NULL)
		delete tmp;
	tmp = new QImage(*image);

	QVector<QRgb> colors = tmp->colorTable();
	QColor destinationColor = color.toHsv();
	for(int i = 0;i < colors.size();i++)
	{
		// Cyan has no red so move on if this color has red.
		if(qRed(colors[i]) != 0 || qAlpha(colors[i]) == 0)
			continue;

		int hue = 0;
		int saturation = 0;
		int value = 0;
		destinationColor.getHsv(&hue, &saturation, &value);
		destinationColor.setHsv(hue, saturation, QColor(colors[i]).toHsv().value());
		colors[i] = destinationColor.rgb();
	}
	tmp->setColorTable(colors);

	return tmp;
}

void PlayersDiagram::loadImages(int style)
{
	if(openImage != NULL)
	{
		delete openImage;
		delete openSpecImage;
		delete botImage;
		delete playerImage;
		delete spectatorImage;
	}

	if(style >= NUM_SLOTSTYLES || style < 0)
	{
		style = 0;
	}

	QString filename(":/slots/");
	filename += slotStyles[style];
	openImage = new QImage(filename + "/open");
	openSpecImage = new QImage(filename + "/specopen");
	botImage = new QImage(filename + "/bot");
	playerImage = new QImage(filename + "/player");
	spectatorImage = new QImage(filename + "/spectator");
}
