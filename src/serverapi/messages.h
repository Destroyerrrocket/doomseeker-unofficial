//------------------------------------------------------------------------------
// messages.h
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
#ifndef __MESSAGES_H_
#define __MESSAGES_H_

#include "global.h"
#include <QString>

/**
 *	@brief Contains information about possible messages generated
 *	by Doomseeker.
 *
 *	Message types are split to informations and errors. All error types
 *	are greater than or equal to Types::CUSTOM_ERROR value. All information
 *	types are greater than or equal to Types::CUSTOM_INFORMATION value and 
 *	lesser than Types::CUSTOM_ERROR value. Alternatively Messages methods
 *	may be used to determine whether a message is error or information.
 *
 *	Types::IGNORE_TYPE is used for dummy Message objects that should be 
 *	dropped.
 */
class MAIN_EXPORT Messages
{
	public:
		class Types
		{
			public:
				static const unsigned IGNORE_TYPE = 0;

				static const unsigned CUSTOM_INFORMATION = 1;

				static const unsigned CUSTOM_ERROR = 0x7fffffff;
		};

		static QString		getString(int type);

		static bool			isError(int type)
		{
			return type >= Types::CUSTOM_ERROR;
		}

		static bool			isIgnore(int type)
		{
			return type == Types::IGNORE_TYPE;
		}

		static bool			isInformation(int type)
		{
			return (type >= Types::CUSTOM_INFORMATION) && (type < Types::CUSTOM_ERROR);
		}
};

/**
 *	@brief Message object used to pass messages throughout the Doomseeker's
 *	system. This should be directly connected to the types provided by the 
 *	Messages class.
 *
 *	Message type and content is carried over through this object. If type
 *	member is set to Messages::Types::IGNORE_TYPE then content member should
 *	be ignored and the entire Message object should be treated as carrying
 *	no message at all.
 */
class MAIN_EXPORT Message
{
	public:
		Message()
		{
			this->type = Messages::Types::IGNORE_TYPE;
		}

		Message(int type)
		{
			this->type = type;
		}

		QString			content;
		unsigned		type;

		bool			isError() const
		{
			return Messages::isError(type);
		}

		bool			isIgnore() const
		{
			return Messages::isIgnore(type);
		}

		bool			isInformation() const
		{
			return Messages::isInformation(type);
		}

		/**
		 *	@brief Convenience method. Sets type 
		 *	to Messages::Types::CUSTOM_ERROR.
		 */
		void			setCustomError(const QString& content)
		{
			setValues(Messages::Types::CUSTOM_ERROR, content);
		}

		/**
		 *	@brief Convenience method. Sets type 
		 *	to Messages::Types::CUSTOM_INFORMATION.
		 */
		void			setCustomInformation(const QString& content)
		{
			setValues(Messages::Types::CUSTOM_INFORMATION, content);
		}

		void			setToIgnore()
		{
			this->type = Messages::Types::IGNORE_TYPE;
		}

		void			setValues(unsigned type, const QString& content)
		{
			this->type = type;
			this->content = content;
		}
};

#endif
