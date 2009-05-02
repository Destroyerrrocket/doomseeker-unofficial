//------------------------------------------------------------------------------
// wadseekerinterface.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WADSEEKERINTERFACE_H_
#define __WADSEEKERINTERFACE_H_

#include "ui_wadseekerinterface.h"
#include "wadseeker/wadseeker.h"

class WadSeekerInterface : public QDialog, Ui::WadSeekerInterface
{
	Q_OBJECT

	public:
		WadSeekerInterface(QWidget* parent = NULL);

	public slots:
		virtual void 	accept();
		void			allDone();
		void			error(const QString&, bool bIsCritical);
		void			notice(const QString&);
		void			wadDone(bool bFound, const QString& wadname);

	protected:
		Wadseeker 		wadseeker;
};

#endif
