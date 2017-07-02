//------------------------------------------------------------------------------
// crc.h
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
// Copyright (C) 2015 "Samuel Villarreal"
//------------------------------------------------------------------------------

#ifndef __KEXCRC_H__
#define __KEXCRC_H__

class kexCRC
{
public:
	static uint32_t crc32_tab[8][256];
	static const uint32_t Mem32(const void *pInData, int32_t length, uint32_t crcHash);
};

#endif