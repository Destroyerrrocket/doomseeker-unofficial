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
class MAIN_EXPORT Message : public QObject
{
    Q_OBJECT

	public:
	    class Types
		{
			public:
				static const unsigned IGNORE_TYPE = 0;

				static const unsigned CUSTOM_INFORMATION = 1;

				static const unsigned CUSTOM_ERROR = 0x7fffffff;
				static const unsigned BANNED_FROM_MASTERSERVER = CUSTOM_ERROR + 1;
		};

        /**
		 *	@brief Convenience method. Sets type
		 *	to Types::CUSTOM_ERROR.
		 */
		static Message	    customError(const QString& content)
		{
		    return Message(Types::CUSTOM_ERROR, content);
		}

		/**
		 *	@brief Convenience method. Sets type
		 *	to Types::CUSTOM_INFORMATION.
		 */
		static Message      customInformation(const QString& content)
		{
			return Message(Types::CUSTOM_INFORMATION, content);
		}

		static QString	    getStringBasingOnType(unsigned type);

        static bool         isCustom(unsigned type)
        {
            return type == Types::CUSTOM_ERROR || type == Types::CUSTOM_INFORMATION;
        }

		static bool			isError(unsigned type)
		{
			return type >= Types::CUSTOM_ERROR;
		}

		static bool			isIgnore(unsigned type)
		{
			return type == Types::IGNORE_TYPE;
		}

		static bool			isInformation(unsigned type)
		{
			return (type >= Types::CUSTOM_INFORMATION) && (type < Types::CUSTOM_ERROR);
		}

		Message()
		{
		    construct();
			this->_type = Types::IGNORE_TYPE;
		}

		~Message() {}

		Message(const Message& other)
		{
            copy(other);
		}

		Message(unsigned type)
		{
			construct();
			this->_type = type;
		}

		Message(unsigned type, const QString& content)
		{
		    construct();
			this->content = content;
			this->_type = type;
		}

		Message& operator=(const Message& other)
		{
            if (this != &other)
            {
                copy(other);
            }

            return *this;
		}

		const QString&  contents() const
		{
		    return content;
		}

        QString		    getStringBasingOnType() const
        {
            return getStringBasingOnType(_type);
        }

        bool            isCustom() const
        {
            return isCustom(_type);
        }

		bool			isError() const
		{
			return isError(_type);
		}

		bool			isIgnore() const
		{
			return isIgnore(_type);
		}

		bool			isInformation() const
		{
			return isInformation(_type);
		}

		unsigned        timestamp() const
		{
		    return _timestamp;
		}

		unsigned        type() const
		{
		    return _type;
		}

    private:
		QString			content;
		unsigned        _timestamp;
		unsigned		_type;

        void            copy(const Message& other)
        {
            this->content = other.content;
            this->_timestamp = other._timestamp;
            this->_type = other._type;
        }

		void            construct();
};

Q_DECLARE_METATYPE(Message)

#endif
