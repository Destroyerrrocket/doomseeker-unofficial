//------------------------------------------------------------------------------
// ip2cupdatebox.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IP2CUPDATEBOX_H__
#define __IP2CUPDATEBOX_H__

#include <QDialog>
#include "ui_ip2cupdatebox.h"

class IP2CUpdateBox : public QDialog, private Ui::IP2CUpdateBox
{
	public:
		IP2CUpdateBox(QWidget* parent = NULL);
		
	protected:
		void			updateInfo();
};

#endif
