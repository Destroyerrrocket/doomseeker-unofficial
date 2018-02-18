//------------------------------------------------------------------------------
// modfile.h
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef ided18842d_ed12_42b5_9c38_c21abea3480f
#define ided18842d_ed12_42b5_9c38_c21abea3480f

#include "dptr.h"
#include "../wadseekerexportinfo.h"

#include <QString>
#include <QUrl>

class WADSEEKER_API ModFile
{
public:
	ModFile();
	virtual ~ModFile();

	const QString &fileName() const;
	void setFileName(const QString &) const;

	const QString &name() const;
	void setName(const QString &) const;

	const QString &description() const;
	void setDescription(const QString &) const;

	const QString &md5() const;
	void setMd5(const QString &) const;

	const QUrl &url() const;
	void setUrl(const QUrl &) const;

	const QString &version() const;
	void setVersion(const QString &) const;

private:
	DPtr<ModFile> d;
};

#endif
