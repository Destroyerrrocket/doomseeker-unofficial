//------------------------------------------------------------------------------
// wadspicker.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef idcb0c6116_a0f7_4c45_bbe4_42e4b1944363
#define idcb0c6116_a0f7_4c45_bbe4_42e4b1944363

#include <QWidget>

class QStringList;

class WadsPicker : public QWidget
{
Q_OBJECT

public:
	WadsPicker(QWidget *parent);
	~WadsPicker();

	QStringList filePaths() const;
	void setFilePaths(const QStringList &paths);

private slots:
	void addWadPath(const QString &wadPath);
	void browseAndAdd();
	void removeAll();
	void removeSelected();

private:
	class PrivData;
	PrivData *d;
};

#endif
