//------------------------------------------------------------------------------
// modset.h
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id78d5de3c_ac88_4972_86bf_667567e4e647
#define id78d5de3c_ac88_4972_86bf_667567e4e647

#include "dptr.h"
#include "modfile.h"
#include "../wadseekerexportinfo.h"

class WADSEEKER_API ModSet
{
public:
	ModSet();
	~ModSet();

	void addModFile(const ModFile &file);
	void clear();
	ModFile findFileName(const QString &fileName) const;
	ModFile first() const;
	bool isEmpty() const;
	QList<ModFile> modFiles() const;
	void removeModFile(const ModFile &file);

private:
	DPtr<ModSet> d;
};

#endif
