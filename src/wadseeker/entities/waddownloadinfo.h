//------------------------------------------------------------------------------
// waddownloadinfo.h
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WADDOWNLOADINFO_H__
#define __WADDOWNLOADINFO_H__

#include <QByteArray>
#include <QString>

/**
 * @brief Holds information about unique WAD download.
 */
class WadDownloadInfo
{
	public:
		WadDownloadInfo(const QString& name);

		/**
 		 * @brief Downloaded contents of the file.
		 */
		const QByteArray& fileData() const
		{
			return d.fileData;
		}

		/**
 		 * @brief Recognizes if download is an archive basing on the extension
		 * in the name specified in the constructor.
		 */
		bool isArchive() const;

		const QString& name() const
		{
			return d.name;
		}

		void setFileData(const QByteArray& fileData)
		{
			d.fileData = fileData;
		}

	private:
		class PrivData
		{
			public:
				QByteArray fileData;
				QString name;
		};

		PrivData d;
};

#endif
