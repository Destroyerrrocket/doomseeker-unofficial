//------------------------------------------------------------------------------
// ircdelayedoperation.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IRCDELAYEDOPERATION_H__
#define __IRCDELAYEDOPERATION_H__

#include <QHash>
#include <QString>

class IRCDelayedOperation
{
	public:
		enum OperationType
		{
			Ban
		};
		
		IRCDelayedOperation(OperationType operationType, const QString& nickname = "", const QString& channel = "");
		
		QString attribute(const QString& name) const
		{
			if (attributes.contains(name))
			{
				return attributes[name];
			}
			
			return QString();
		}
		
		const QString& channel() const { return channelName; }
		const QString& nickname() const { return nick; }
		OperationType operationType() const { return operType; }
		
		bool operator==(const IRCDelayedOperation& other) const;
		void setAttribute(const QString& name, const QString& value)
		{
			if (attributes.contains(name))
			{
				attributes[name] = value;
			}
			else
			{
				attributes.insert(name, value);
			}
		}
		
	
	private:
		QHash<QString, QString> attributes;
		QString channelName;
		QString nick;
		OperationType operType;
};

#endif
