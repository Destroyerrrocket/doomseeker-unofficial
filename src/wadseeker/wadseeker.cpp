//------------------------------------------------------------------------------
// wadseeker.cpp
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

#include "wadseeker.h"

Wadseeker::Wadseeker()
{
	connect(&http, SIGNAL( dataReceived() ), this, SLOT( pageReceived() ) );
}

Wadseeker::~Wadseeker()
{
}

bool Wadseeker::seekWads(QStringList& wads)
{
	http.setSite("zalewa.dyndns.org");
	http.sendRequestGet("/robert/pic/index.php");

	return true;
}

void Wadseeker::pageReceived()
{
	QList<Link> list = http.links();
	QList<Link>::iterator it;
	for (it = list.begin(); it != list.end(); ++it)
	{
		qDebug() << it->url.toAscii().constData() << it->text.toAscii().constData();
	}
}

