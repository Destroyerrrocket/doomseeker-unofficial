//------------------------------------------------------------------------------
// unarchive.h
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __UNARCHIVE_H__
#define __UNARCHIVE_H__

#include <QObject>

class UnArchive : public QObject
{
	Q_OBJECT

	public:
		virtual bool	extract(int file, const QString &where)=0;
		virtual QString	fileNameFromIndex(int file)=0;
		virtual int		findFileEntry(const QString &entryName)=0;
		virtual bool	isValid()=0;

	signals:
		void			message(const QString&, int type);
};

#endif /* __UNARCHIVE_H__ */
