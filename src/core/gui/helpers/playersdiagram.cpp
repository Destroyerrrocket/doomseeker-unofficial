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
#include "serverapi/playerslist.h"
#include "serverapi/server.h"
#include "log.h"
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

	obtainPlayerNumbers();
	draw();
}

PlayersDiagram::~PlayersDiagram()
{
	if(tmp != NULL)
		delete tmp;
}

const QImage* PlayersDiagram::colorizePlayer(const QImage *image, const QColor &color)
{
	if(tmp != NULL)
	{
		delete tmp;
	}
	tmp = new QImage(*image);

	QVector<QRgb> colors = tmp->colorTable();
	QColor destinationColor = color.toHsv();
	for(int i = 0; i < colors.size(); ++i)
	{
		// Cyan has no red so move on if this color has red.
		if(qRed(colors[i]) != 0 || qAlpha(colors[i]) == 0)
		{
			continue;
		}

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

void PlayersDiagram::deleteImages()
{
	if(openImage != NULL)
	{
		delete openImage;
		delete openSpecImage;
		delete botImage;
		delete playerImage;
		delete spectatorImage;
	}
}

void PlayersDiagram::draw()
{
	// Don't bother trying to draw an empty image.
	if(server->numTotalSlots() == 0)
		return;

	diagram = QPixmap(server->numTotalSlots() * playerImage->width(), playerImage->height());
	diagram.fill(Qt::transparent);

	slotSize = playerImage->width();
	position = diagram.width() - slotSize;
	painter = new QPainter(&diagram);

	for (int team = 0; team < MAX_TEAMS; ++team)
	{
		drawTeam(Human, team, numHumansOnTeam[team]);
		drawTeam(Bot, team, numBotsOnTeam[team]);
	}

	drawTeam(Human, Player::TEAM_NONE, numHumansWithoutTeam);
	drawTeam(Bot, Player::TEAM_NONE, numBotsWithoutTeam);

	if (numSpectators > 0)
	{
		drawPictures(spectatorImage, numSpectators);
	}

	if (numFreeJoinSlots > 0)
	{
		drawPictures(openImage, numFreeJoinSlots);
	}

	if (numFreeSpectatorSlots > 0)
	{
		drawPictures(openSpecImage, numFreeSpectatorSlots);
	}

	delete painter;
}

void PlayersDiagram::drawTeam(PlayerType playerType, int team, int howMany)
{
	if (howMany > 0)
	{
		const QImage* baseImage;

		switch(playerType)
		{
			case Bot:
				baseImage = botImage;
				break;

			case Human:
				baseImage = playerImage;
				break;

			default:
				gLog << "Error inside PlayersDiagram::drawTeam(): unknown PlayerType";
				return;
		}

		const QImage* picture = colorizePlayer(baseImage, QColor(server->teamColor(team)));
		drawPictures(picture, howMany);
	}
}

void PlayersDiagram::drawPictures(const QImage* image, int howMany)
{
	assert(image != NULL);
	for (; howMany > 0; --howMany)
	{
		painter->drawImage(position, 0, *image);
		position -= slotSize;
	}
}

bool PlayersDiagram::isStyleNumberValid(int style)
{
	return style >= NUM_SLOTSTYLES || style < 0;
}

void PlayersDiagram::loadImages(int style)
{
	deleteImages();

	if(isStyleNumberValid(style))
	{
		style = 0;
	}

	QString filepath(":/slots/");
	filepath += slotStyles[style];
	openImage = new QImage(filepath + "/open");
	openSpecImage = new QImage(filepath + "/specopen");
	botImage = new QImage(filepath + "/bot");
	playerImage = new QImage(filepath + "/player");
	spectatorImage = new QImage(filepath + "/spectator");
}

void PlayersDiagram::obtainPlayerNumbers()
{
	memset(numBotsOnTeam, 0, sizeof(int) * MAX_TEAMS);
	memset(numHumansOnTeam, 0, sizeof(int) * MAX_TEAMS);

	const PlayersList* playersList = server->players();

	numBotsWithoutTeam = playersList->numBotsWithoutTeam();
	numFreeJoinSlots = server->numFreeJoinSlots();
	numFreeSpectatorSlots = server->numFreeSpectatorSlots();
	numHumansWithoutTeam = playersList->numHumansWithoutTeam();
	numSpectators = playersList->numSpectators();

	for(int i = 0; i < MAX_TEAMS; ++i)
	{
		numBotsOnTeam[i] = playersList->numBotsOnTeam(i);
		numHumansOnTeam[i] = playersList->numHumansOnTeam(i);
	}
}
