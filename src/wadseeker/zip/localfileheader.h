//------------------------------------------------------------------------------
// zipfileheader.h
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
#ifndef __LOCALFILEHEADER_H_
#define __LOCALFILEHEADER_H_

#include <QByteArray>
#include <QString>

#define READINT32(pointer) ((quint32((quint8)(*pointer))) | (quint32(quint8(*(pointer+1)))<<8) | (quint32(quint8(*(pointer+2)))<<16) | (quint32(quint8(*(pointer+3)))<<24))
#define READINT16(pointer) ((quint16((quint8)(*pointer))) | (quint16(quint8(*(pointer+1)))<<8))

#define LOCAL_FILE_HEADER_SIGNATURE 0x04034b50
#define DESCRIPTOR_EXISTS_FLAG		0x0004 // (3rd bit)
#define ZIP_LOCAL_FILE_HEADER_SIZE	30

struct ZipLocalFileHeader
{
	enum HeaderError
	{
		NoError 				= 0,
		Corrupted				= 1,
		NotLocalFileHeader 		= 2
	};

	ZipLocalFileHeader()
	{
		localFileHeaderSignature = 0;
	}

	ZipLocalFileHeader(const ZipLocalFileHeader& copyin)
	{
		localFileHeaderSignature = 0;

		copy(copyin);
	}

	~ZipLocalFileHeader()
	{
	}

	/**
	 * Calculated depending on below variables.
	 */
	unsigned long	fileEntrySize() const
	{
		long dataDescriptorSize = 0;
		if (generalPurposeBitFlag & DESCRIPTOR_EXISTS_FLAG == DESCRIPTOR_EXISTS_FLAG)
		{
			dataDescriptorSize = 12;
		}

		return howManyBytesTillData() + dataDescriptorSize + compressedSize;
	}

	/**
	 * @param data - byte array
	 * @param dataSize - size of the byte array, function will return true if this is equal to 30.
	 * @return If incorrect data is passed (this could happen if the zip file is corrupted)
	 * it will set localFileHeaderSignature to correct value but still return false.
	 * If given data is not a local file header it will also return false but localFileHeaderSignature
	 * will be set to other value.
	 */
	int				fromByteArray(const QByteArray& array)
	{
		const char* data = array.constData();

		localFileHeaderSignature = 0;

		if (array.size() < 4)
			return NotLocalFileHeader;

		localFileHeaderSignature 	= READINT32(data);

		if (localFileHeaderSignature != LOCAL_FILE_HEADER_SIGNATURE)
			return NotLocalFileHeader;

		// The header is correct but the data size is not enough.
		if (array.size() != 30)
		{
			return Corrupted;
		}

		versionNeededToExtract 		= READINT16(&data[4]);
		generalPurposeBitFlag		= READINT16(&data[6]);
		compressionMethod			= READINT16(&data[8]);
		lastModFileTime				= READINT16(&data[10]);
		lastModFileDate				= READINT16(&data[12]);
		crc32						= READINT32(&data[14]);
		compressedSize				= READINT32(&data[18]);
		uncompressedSize			= READINT32(&data[22]);
		fileNameLength				= READINT16(&data[26]);
		extraFieldLength			= READINT16(&data[28]);

		return NoError;
	}

	unsigned long	howManyBytesTillData() const
	{
		return ZIP_LOCAL_FILE_HEADER_SIZE + fileNameLength + extraFieldLength;
	}

    ZipLocalFileHeader& operator=(const ZipLocalFileHeader& rhs)
    {
    	if (this == &rhs)
			return *this;

		copy(rhs);
    }

	/* FROM ZIP FILE FORMAT DESCRIPTION */
	unsigned long 	localFileHeaderSignature; // (0x04034b50), if anything fails this will be set to 0
	unsigned short 	versionNeededToExtract;
    unsigned short 	generalPurposeBitFlag;
    unsigned short 	compressionMethod;
    unsigned short 	lastModFileTime;
	unsigned short 	lastModFileDate;
    unsigned long  	crc32;
    unsigned long  	compressedSize;
	unsigned long	uncompressedSize;
    unsigned short	fileNameLength;
	unsigned short	extraFieldLength;

    QString			fileName;
    QByteArray		extraField;
    /* END OF ZIP FILE FORMAT DESCRIPTION */

	/**
     * Position in the ZIP file at which the header begins.
     * Data position can be calculated from this using
     * howManyBytesTillData() method.
     */
	unsigned long 	headerPosition;

    protected:
		void copy(const ZipLocalFileHeader& o)
		{
			localFileHeaderSignature = o.localFileHeaderSignature;
			versionNeededToExtract = o.versionNeededToExtract;
			generalPurposeBitFlag = o.generalPurposeBitFlag;
			compressionMethod = o.compressionMethod;
			lastModFileTime = o.lastModFileTime;
			lastModFileDate = o.lastModFileDate;
			crc32 = o.crc32;
			compressedSize = o.compressedSize;
			uncompressedSize = o.uncompressedSize;
			fileNameLength = o.fileNameLength;
			extraFieldLength = o.extraFieldLength;

			fileName = o.fileName;
			extraField = o.extraField;

			headerPosition = o.headerPosition;
		}
};

#endif
