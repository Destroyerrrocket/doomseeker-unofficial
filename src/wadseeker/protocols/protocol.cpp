//------------------------------------------------------------------------------
// protocol.cpp
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
#include "protocol.h"
#include <QFileInfo>

Protocol::Protocol()
{
	connect(&timeoutTimer, SIGNAL( timeout() ), this, SLOT( timeout() ) );

	timeoutTimer.setSingleShot(true);
}

void Protocol::abort()
{
	aborting = true;
	timeoutTimer.stop();
	abortEx();
}

void Protocol::dataReadProgressSlot(int done, int total)
{
	timeoutTimer.start(WWW_TIMEOUT_MS);
	emit dataReadProgress(done, total);
}

void Protocol::doneSlot(bool error)
{
	timeoutTimer.stop();
	if (!aborting)
	{
		doneEx(error);
	}
}

void Protocol::get(const QUrl& url)
{
	aborting = false;
	QFileInfo fi(url.path());
	processedFileName = fi.fileName();

	timeoutTimer.start(WWW_TIMEOUT_MS);

	getEx(url);
}

void Protocol::timeout()
{
	emit message(tr("Request timeout!\n"), Error);
	abort();
}
