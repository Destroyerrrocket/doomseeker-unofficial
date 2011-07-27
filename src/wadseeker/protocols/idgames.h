//------------------------------------------------------------------------------
// idgames.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IDGAMES_H_
#define __IDGAMES_H_

#include "http.h"

/**
 *  This works a bit different than Http class.
 *  First of all if a HTML page is received it is not
 *  necessarily sent through done() signal.
 *  Instead it is processed internally. If Idgames instance
 *  is still in "Idgames searching" state the page will be processed
 *  internally. If page describing requested file is found, this page
 *  is then sent as HTML through done() signal. It can then be
 *  processed as usual by WWWSeeker class.
 */
//class Idgames : public Http
//{
//	Q_OBJECT
//
//	public:
//		static QString defaultIdgamesUrl();
//
//		/**
//		 *	Creates object to handle Idgames archive.
//		 *	@param idgamesPage - address of idgames archive.
//		 *		this should contain %ZIPNAME% and %PAGENUM%
//		 *		to find the file and iterate through pages properly.
//		 */
//		Idgames(const QString& idgamesPage);
//
//		/**
//		 *	Entry method. From this point the whole process can go
//		 *	in 4 ways:
//		 *	a)	program iterates through sites until it finds the requested
//		 *		file, then a site for this file is returned through done()
//		 *		signal
//		 *	b)	program iterates through sites and stumbles upon a page with
//		 *		no file entries. done() signal with "fail" flag is sent.
//		 *	c)	no %PAGENUM% is present in URL, search is done only once and
//		 *		then it fails immediately after trying to go to next page
//		 *		(in case if the file is not found after this first search)
//		 *	d)	no %ZIPNAME% is present in URL, done() with "fail" flag is
//		 *		sent immediately
//		 */
//		void findFile(const QString& zipName);
//
//		void setPage(const QString& url) { idgamesBaseUrl = url; }
//
//	protected:
//		enum PageProcessResults
//		{
//			NotIdgames 		= -2,
//			StringTooShort 	= -1,
//			NoPositions		= 0,
//			Ok				= 1,
//		};
//
//		int 		currentPage;
//		bool        filePageFound;
//		QString		idgamesBaseUrl;
//		QString		seekedFile;
//
//		/**
//		 *	This should be called immediately after processPage.
//		 *	@param response - return of processPage()
//		 *	@param url - url param of processPage()
//		 */
//		void				afterProcess(PageProcessResults result, const QString& url);
//
//		/**
//		 *	Downloads Idgames page and increases currentPage counter.
//		 */
//		void				getPage();
//
//		void				doneEx(bool error);
//		/**
//		 *	@param [out] url - link to the page describing the file, or empty
//		 *		string if file not found
//		 */
//		PageProcessResults	processPage(QByteArray& pageData, QString& url);
//};

#endif
