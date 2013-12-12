//------------------------------------------------------------------------------
// chocolatedoomgamehost.h
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
#ifndef id99357B79_ABB5_45CE_AEA817B384EFC912
#define id99357B79_ABB5_45CE_AEA817B384EFC912

#include <serverapi/gamehost.h>

class ChocolateDoomServer;

class ChocolateDoomGameHost : public GameHost
{
	Q_OBJECT

	public:
		ChocolateDoomGameHost(const ChocolateDoomServer* server);

	protected:
		void hostProperties(QStringList& args) const;

	private:
		Q_DISABLE_COPY(ChocolateDoomGameHost)

		const ChocolateDoomServer* server;
};

#endif
