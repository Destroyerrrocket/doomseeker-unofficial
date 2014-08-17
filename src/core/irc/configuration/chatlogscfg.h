//------------------------------------------------------------------------------
// chatlogscfg.h
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
#ifndef id46ac54ff_adba_49cd_b972_b804898b4fb6
#define id46ac54ff_adba_49cd_b972_b804898b4fb6

#include <QString>
#include <QVariant>

class ChatLogsCfg
{
public:
	ChatLogsCfg();
	~ChatLogsCfg();

	QString chatLogsRootDir() const;
	void setChatLogsRootDir(const QString &val);

	bool isStoreLogs() const;
	void setStoreLogs(bool b);

	bool isRestoreChatFromLogs() const;
	void setRestoreChatFromLogs(bool b);


private:
	class PrivData;
	PrivData *d;

	void setValue(const QString &key, const QVariant &value);
	QVariant value(const QString &key, const QVariant &defValue = QVariant()) const;
};

#endif
