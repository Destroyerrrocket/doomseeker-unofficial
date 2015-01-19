//------------------------------------------------------------------------------
// playerdiagram.h
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
#ifndef __PLAYERS_DIAGRAM_H_
#define __PLAYERS_DIAGRAM_H_

#include "serverapi/serverptr.h"
#include "serverapi/player.h"
#include <QPixmap>

class Server;
class QImage;

#define NUM_SLOTSTYLES 2

class PlayersDiagram
{
	public:
		/**
		 *	Loads all the images used to build a diagram. Previous images
		 *	will be freed. This is be used to change the diagram appearance
		 *	when Configuration box indicates that such action is required.
		 */
		static void loadImages(int style);

		PlayersDiagram(ServerCPtr server);

		~PlayersDiagram();

		QPixmap pixmap() const { return diagram; }

	protected:
		enum PlayerType
		{
			Bot,
			Human
		};

		static void deleteImages();
		static bool isStyleNumberValid(int style);

		static const char* slotStyles[NUM_SLOTSTYLES];
		static const QImage *openImage, *openSpecImage, *botImage, *playerImage, *spectatorImage;

		/**
		 * Colorizes the image to color.  This works is a fairly hacky way.  It
		 * colorizes cyan areas only.  To detect cyan it simply checks if a red
		 * component is present.  If so it is not cyan.
		 *
		 * Colorization is done by keeping the hue and saturation if the passed
		 * in color and applying the value of the color in the image.
		 */
		const QImage* colorizePlayer(const QImage *image, const QColor &color);

		void draw();
		void drawTeam(PlayerType playerType, int team, int howMany);
		void drawPictures(const QImage* image, int howMany);

		void obtainPlayerNumbers();

		int numBotsOnTeam[MAX_TEAMS];
		int numBotsWithoutTeam;
		int numFreeJoinSlots;
		int numFreeSpectatorSlots;
		int numHumansWithoutTeam;
		int numHumansOnTeam[MAX_TEAMS];
		int numSpectators;

	private:
		ServerCPtr server;
		QPixmap diagram;
		QPainter* painter;
		int position;
		int slotSize;
		QImage *tmp;
};

#endif
