//------------------------------------------------------------------------------
// ftp.h
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
#ifndef __FTP_H_
#define __FTP_H_

#include <QFtp>
#include "protocol.h"

class Ftp : public Protocol
{
	public:
		static bool	isFTPLink(const QUrl&);

		/**
		 * Info for QFtp needs to be specified more explictly that info for QHttp.
		 * This method takes care of splitting proper info from QUrl and
		 * subsequently it starts communicating with FTP server.
		 */

	protected:
		QString		defaultScheme() { return "ftp"; }
		void		sendGet();

		QFtp	qFtp;
};

#endif
