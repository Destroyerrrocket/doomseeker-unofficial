//------------------------------------------------------------------------------
// ircignoresmanager.h
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
#ifndef id16362687_1562_4529_ac24_a94f29a52c9f
#define id16362687_1562_4529_ac24_a94f29a52c9f

#include <QDialog>
#include "ui_ircignoresmanager.h"

class PatternList;

class IRCIgnoresManager : public QDialog, private Ui::IRCIgnoresManager
{
Q_OBJECT

public:
	IRCIgnoresManager(QWidget *parent, const QString &networkDescription);
	~IRCIgnoresManager();

protected:
	void done(int result);
	void keyPressEvent(QKeyEvent *event);

private:
	class PrivData;
	PrivData *d;

	void loadItems();
	PatternList patterns() const;
	void saveItems();

private slots:
	void deleteSelected();
};

#endif
