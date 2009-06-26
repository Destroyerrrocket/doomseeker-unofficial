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

class Idgames : public Http
{
	Q_OBJECT

	public:
		/**
		 *	Creates object to handle Idgames archive.
		 *	@param idgamesPage - address of idgames archive.
		 *		this should contain %ZIPNAME% and %NUMPAGE%
		 *		to find the file and iterate through pages properly.
		 */
		Idgames(const QString& idgamesPage);

		void findFile(const QString& zipName);

	protected:
		QString		idgamesBaseUrl;
		QString		seekedFile;

	private:

};

#endif
