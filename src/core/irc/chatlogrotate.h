//------------------------------------------------------------------------------
// chatlogrotate.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef ida3366499_a83b_4d70_908c_8d0d2a7e3e36
#define ida3366499_a83b_4d70_908c_8d0d2a7e3e36

#include <QFileInfo>
#include <QString>

class IRCNetworkEntity;

class ChatLogRotate
{
public:
	ChatLogRotate();
	~ChatLogRotate();

	void setMaxSize(int size);
	/**
	 * Setting this to a negative number disables the removal.
	 */
	void setRemovalAgeDaysThreshold(int age);

	void rotate(const IRCNetworkEntity &network, const QString &recipient);

private:
	class PrivData;
	PrivData *d;

	void archivizeCurrent(const IRCNetworkEntity &network, const QString &recipient);
	void mkBackupDir(const IRCNetworkEntity &network, const QString &recipient);
	void purgeOld(const IRCNetworkEntity &network, const QString &recipient);

	bool isEligibleForRemoval(const QFileInfo &entry) const;
};

#endif
