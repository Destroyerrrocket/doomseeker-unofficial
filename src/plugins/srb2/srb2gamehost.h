//------------------------------------------------------------------------------
// srb2gamehost.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2016 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id0C2CBB3A_3E88_4063_9F380FB52C71E32A
#define id0C2CBB3A_3E88_4063_9F380FB52C71E32A

#include <serverapi/gamehost.h>
#include <QList>

class DMFlagsSection;

class Srb2GameHost : public GameHost
{
	Q_OBJECT

	public:
		Srb2GameHost();

	protected:
		void addExtra();

	private:
		Q_DISABLE_COPY(Srb2GameHost)

		bool casualServer;
		bool listenServer;

		void addDMFlags();
		void addDMFlags(const QList<DMFlagsSection> &flags, bool enabled);
		void addIwad();
};

#endif
