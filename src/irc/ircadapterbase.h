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
		
	public:
		/**
		 *	@brief Implement to handle and send a message to the IRC network
		 *	entity.
		 *
		 *	This is meant for internal use by the IRCAdapterBase derivatives.
		 *
		 *	Each implementation treats this method a bit differently.
		 *	Please refer to the documentation in the classes that derive from
		 *	this one.
		 *
		 *	@param pOrigin
		 *		Origin of this message. Can be used to determine where the error
		 *		and message signals should be passed.
		 */	
		virtual void		doSendMessage(const QString& message, IRCAdapterBase* pOrigin) = 0;
		
		void				emitError(const QString& strError)
		{
			emit error(strError);
		}
		
		void				emitMessage(const QString& strMessage)
		{
			emit message(strMessage);
		}
			
	public slots:
		void				sendMessage(const QString& message)
		{
			doSendMessage(message, NULL);
		}
	
	signals:
		void				error(const QString& error);
		void				message(const QString& message);
};

#endif
