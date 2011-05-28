//------------------------------------------------------------------------------
// vavooserver.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __ZDAEMONSERVER_H__
#define __ZDAEMONSERVER_H__

#include "serverapi/server.h"

#define ZD_MAX_TEAMS 4

class Binaries;
class GameRunner;
class PluginInfo;

class TeamInfo
{
	public:
		TeamInfo(const QString &name=QObject::tr("<< Unknown >>"), const QColor &color=QColor(0,0,0), unsigned int score=0);

		const QString	&name() const { return teamName; }
		const QColor	&color() const { return teamColor; }
		unsigned int	score() const { return teamScore; }

	protected:
		void			setName(const QString &name) { teamName = name; }
		void			setColor(const QColor &color) { teamColor = color; }
		void			setScore(int score) { teamScore = score; }

		friend class	ZDaemonServer;

	private:
		QString			teamName;
		QColor			teamColor;
		unsigned int	teamScore;
};

class ZDaemonServer : public Server
{
	Q_OBJECT

	public:
		static const QPixmap			*ICON;

		ZDaemonServer(const QHostAddress &address, unsigned short port);

		Binaries*			binaries() const;
		GameRunner*			gameRunner() const;

		const QPixmap&		icon() const;

		const PluginInfo*	plugin() const;

		QRgb				teamColor(int team) const;
		QString				teamName(int team) const;

	protected:
		Response	readRequest(QByteArray &data);
		bool		sendRequest(QByteArray &data);

		static unsigned int	millisecondTime();

		TeamInfo			teamInfo[ZD_MAX_TEAMS];
};

#endif /* __ZDAEMONSERVER_H__ */
