//------------------------------------------------------------------------------
// log.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __LOG_H_
#define __LOG_H_

#define pLog Log::pLogInst

#include "global.h"
#include <QObject>

/**
 *	@brief Log manager.
 */
class MAIN_EXPORT Log : public QObject
{
	Q_OBJECT;

	public:
		/**
		 *	@brief Global instance of the logger.
		 */
		static Log			pLogInst;

		Log();

		bool				areTimestampsEnabled() const { return timestamps; }

		const QString&		content() const { return logContent; }

		/**
		 *	Works like printf() from stdio. After the output string is
		 *	constructed it is passed to addEntry() where additional formatting
		 *	is applied.
		 */
		void				logPrintf(const char* str, ...);

		/**
		 *	Works like printf() from stdio. After the output string is
		 *	constructed it is displayed AS IS, without any additional
		 *	formatting.
		 */
		void				logUnformattedPrintf(const char* str, ...);

		bool				isPrintingToStdout() const { return printToStdout; }

		void				setPrintingToStdout(bool b) { printToStdout = b; }
		void 				setTimestampsEnabled(bool b) { timestamps = b; }

		/**
		 *	@brief Executes addEntry(const QString&).
		 */
		Log& 				operator<<(const QString& string);

	public slots:
		/**
		 *	Prints the string to specified output and appends a '\\n' character
		 *	to the end of that string. Additional formatting is also applied
		 *	if certain flags are enabled.
		 *	@see newEntry()
		 */
		void	addEntry(const QString& string);

		/**
		 *	Prints the string to specified output AS IT IS.
		 *	@see addEntry()
		 *	@see newEntry()
		 */
		void	addUnformattedEntry(const QString& string);

		/**
		 *	@brief Clears log content stored in the memory.
		 */
		void	clearContent() { logContent.clear(); }

	signals:
		/**
		 *	Sends out an already formatted new entry. Please note that there
		 *	are no additional newline character appended to the entry string.
		 *	This is emitted by addEntry() and addUnformattedEntry() slots.
		 */
		void	newEntry(const QString& entry);

	protected:
		/**
		 *	@brief Entire content of the log.
		 */
		QString		logContent;

		int			doLogPrintf(char* output, unsigned outputSize, const char* str, va_list argList);

		/**
		 *	If true all new entries will also be printed to stdout. Otherwise
		 *	entries are stored only in the logContent member.
		 *	Default is true.
		 */
		bool		printToStdout;

		/**
		 *	@brief Timestamps are in format [hh:mm:ss]. Enabled by default.
		 */
		bool		timestamps;


};

#endif
