//------------------------------------------------------------------------------
// freedoom.h
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
#ifndef idb06bc70b_1d65_4502_98e1_3492d2b760e0
#define idb06bc70b_1d65_4502_98e1_3492d2b760e0

#include "dptr.h"
#include "wadseekerexportinfo.h"
#include <QObject>

class ModSet;

class WADSEEKER_API Freedoom : public QObject
{
	Q_OBJECT;

public:
	Freedoom(QObject *parent = 0);
	virtual ~Freedoom();

	const QString &error() const;
	bool isError() const;
	const ModSet &modSet() const;
	void requestModSet();

signals:
	void finished();

private:
	DPtr<Freedoom> d;
};

#endif
