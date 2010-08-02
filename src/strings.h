//------------------------------------------------------------------------------
// strings.h
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
#ifndef __STRINGS_H_
#define __STRINGS_H_

#include <QString>
#include "global.h"

class MAIN_EXPORT Strings
{
	public:
		/**
		 *	@brief Creates a random string with specified length.
		 *
		 *	The created string consists of lowercase letters and digits.
		 */
		static QString			createRandomAlphaNumericString(unsigned numChars);

		/**
		 * @brief Adds escape characters to a string.
		 */
		static const QString	&escape(QString &str);

		/**
		 *	@brief Similar to formatDataSpeed().
		 *
		 *	The only difference is the unit type appended to the end of the
		 *	returned string.
		 */
		static QString			formatDataAmount(int bytes);
	
		/**
		 *	@brief Formats a numerical speed value into a string.
		 *
		 *	This expects the parameter to represent speed in bytes per second.
		 *	Data will be formatted into human readable format, ie. if speed
		 *	is high enough it will be translated into kilobytes per second
		 *	or megabytes per second. Also a string representing the type of
		 *	value is appended: MB/s, kB/s or B/s.
		 */
		static QString			formatDataSpeed(float speedInBytesPerSecond);

		/**
		 *	@brief Formats a numerical time value into a string.
		 *
		 *	Parameter is expected to be an amount of seconds. Output is
		 *	transformed into a number of hours, minutes and seconds.
		 *	The function will always try to generate the shortest possible
		 *	string, ie. there will be no hour information if seconds parameter
		 *	is smaller than 3600. The same goes for minutes.
		 *	Factorial part in the parameter represents miliseconds and is always
		 *	rounded up to a full second.
		 */
		static QString			formatTime(float seconds);

		static bool				isCharOnCharList(char c, const QString& charList);

		/**
		 *	@brief Translates string in format "hostname:port" to atomic values.
		 *	@param addressString - string to be translated.
		 *	@param[out] hostname - hostname output.
		 *	@param[out] port - port output.
		 *	@param defaultHostname - default hostname to use if translation fails.
		 *	@param defaultPort - default port to use if translation fails
		 *		or no port inside addressString is specified.
		 */
		static void				translateServerAddress(const QString& addressString, QString& hostname, unsigned short& port, const QString& defaultHostname, const unsigned short defaultPort);

		static const QString	&unescape(QString &str);

		/**
		 *	@brief Unsafe URLs begin with file:// and this functions returns
		 *	false for such URLs.
		 *
		 *	Someone might prepare such URL to make Doomseeker activate files on 
		 *	user's local drive and possibly cause damage to the system.
		 */
		static bool				isUrlSafe(const QString& url);

		static QString&			trim(QString& str, const QString& charList) { return trimr(triml(str, charList), charList); }
		static QString&			trimr(QString& str, const QString& charList);
		static QString&			triml(QString& str, const QString& charList);
		
	protected:
		enum DataUnit
		{
			Byte		= 0,
			Kilobyte	= 1,
			Megabyte	= 2,
			Gigabyte	= 3
		};
		
		static const unsigned	RANDOM_CHAR_POOL_SIZE = 36;
		static const char		RANDOM_CHAR_POOL[RANDOM_CHAR_POOL_SIZE];
		
	
		static float			scaleDataUnit(float bytes, DataUnit& outUnit);
};

#endif
