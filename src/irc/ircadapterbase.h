//------------------------------------------------------------------------------
// ircadapterbase.h
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
#ifndef __IRCADAPTERBASE_H__
#define __IRCADAPTERBASE_H__

#include <QObject>

/**
 *	@brief Provides an unified communication interface between a client and
 *	IRC network entities.
 */
class IRCAdapterBase : public QObject
{
	Q_OBJECT
	
	public slots:
		/**
		 *	@brief Implemented to handle and send a message to the IRC network
		 *	entity.
		 *
		 *	Each implementation treats this slot a bit differently.
		 *	Please refer to the documentation in the classes that derive from
		 *	this one.
		 */
		virtual void		sendMessage(const QString& error) = 0;
	
	signals:
		void				error(const QString& error);
		void				message(const QString& message);
};

#endif
