//------------------------------------------------------------------------------
// zandronummasterclient.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net> (skulltagmasterclient.h)
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_PLUGIN_ZANDRONUMMASTERCLIENT_H
#define DOOMSEEKER_PLUGIN_ZANDRONUMMASTERCLIENT_H

#include <serverapi/masterclient.h>

class EnginePlugin;

class ZandronumMasterClient : public MasterClient
{
	Q_OBJECT

	public:
		ZandronumMasterClient();

		const EnginePlugin *plugin() const;

	public slots:
		void refreshStarts();

	protected:
		QByteArray createServerListRequest();
		Response readMasterResponse(const QByteArray &data);

		/// This is used as a marker to determine if we have an accurate reading for numPackets.
		bool readLastPacket;
		int numPackets;
		unsigned int packetsRead;
};

#endif
