//------------------------------------------------------------------------------
// message.h
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
#ifndef __DS_MESSAGE_H_
#define __DS_MESSAGE_H_

#include "global.h"
#include <QDateTime>
#include <QMetaType>
#include <QString>

class StaticMessages : public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Gets static string for a Message::Type.
		 *
		 * @param messageType
		 *     One of constant values defined in Message::Type.
		 */
		static QString getMessage(unsigned messageType);
};

/**
 * @brief Message object used to pass messages throughout the Doomseeker's
 *        system.
 *
 * Message type and content is carried over through this object. If type
 * member is set to Messages::Types::IGNORE_TYPE then content member should
 * be ignored and the entire Message object should be treated as carrying
 * no message at all.
 */
class MAIN_EXPORT Message
{
	public:
		class Type
		{
			public:
				static const unsigned IGNORE_TYPE = 0;

				static const unsigned CUSTOM_INFORMATION = 1;

				static const unsigned CUSTOM_ERROR = 0x7fffffff;
				static const unsigned BANNED_FROM_MASTERSERVER = CUSTOM_ERROR + 1;
		};

		/**
		 * @brief Convenience method. Sets type to Type::CUSTOM_ERROR.
		 */
		static Message customError(const QString& content)
		{
			return Message(Type::CUSTOM_ERROR, content);
		}

		/**
		 * @brief Convenience method. Sets type to Type::CUSTOM_INFORMATION.
		 */
		static Message customInformation(const QString& content)
		{
			return Message(Type::CUSTOM_INFORMATION, content);
		}

		Message();
		Message(unsigned type);
		Message(const Message &other);
		Message &operator=(const Message &other);
		virtual ~Message();

		QString contents() const;

		bool isCustom() const;
		bool isError() const;
		bool isIgnore() const;
		bool isInformation() const;

		unsigned timestamp() const;
		unsigned type() const;

	private:
		class PrivData;
		PrivData *d;

		Message(unsigned type, const QString &content);

		void construct();
};

Q_DECLARE_METATYPE(Message)

#endif
