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

#include "protocol.h"
#include <QFtp>

class Ftp : public Protocol
{
	Q_OBJECT

	public:
		static bool	isFTPLink(const QUrl&);

		Ftp();

	protected slots:
		void	dataTransferProgressSlot(qint64 done, qint64 total);
		void	stateChanged(int);

	protected:
		void 	abortEx();
		void	disconnectQFtp();
		void	doneEx(bool error);
		void 	getEx(const QUrl&);

		QFtp*	qFtp;
		QUrl	queryUrl;
};

#endif
