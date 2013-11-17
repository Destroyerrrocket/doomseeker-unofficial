//------------------------------------------------------------------------------
// passwordscfg.h
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id1E18D75F_28FA_48A1_9CACE55638B41394
#define id1E18D75F_28FA_48A1_9CACE55638B41394

#include <QList>
#include <QString>
#include <QStringList>

class ServerPassword;

class PasswordsCfg
{
	public:
		PasswordsCfg();
		~PasswordsCfg();

		bool isHidingPasswords() const;
		bool isRememberingConnectPhrase() const;
		QString lastUsedConnectPhrase() const;
		void removeServerPhrase(const QString& phrase);
		void saveServerPhrase(const QString& phrase, const QString& serverName,
			const QString& engineName);
		QList<ServerPassword> serverPasswords() const;
		QStringList serverPhrases() const;
		void setHidePasswords(bool val);
		void setLastUsedConnectPhrase(const QString& val);
		void setRememberConnectPhrase(bool val);
		void setServerPasswords(const QList<ServerPassword>& val);

	private:
		class PrivData;
		PrivData* d;
};

#endif
