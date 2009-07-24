//------------------------------------------------------------------------------
// createserver.h
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
#ifndef __CREATESERVER_H_
#define __CREATESERVER_H_

#include "ui_createserver.h"
#include "sdeapi/pluginloader.hpp"
#include <QDialog>

/**
 *	@brief Dialog window allowing user to host a game.
 */
class CreateServerDlg : public QDialog, private Ui::CreateServerDlg
{
	Q_OBJECT

	public:
		CreateServerDlg(QWidget* parent = NULL);
		~CreateServerDlg();

	protected slots:
		void 	btnClicked(QAbstractButton *button);
		void	btnAddPwadClicked();
		void	btnIwadBrowseClicked();
		void	btnRemovePwadClicked();
		void	cboEngineSelected(int index);

	protected:
		const PluginInfo* 		currentEngine;

		void	addWadPath(const QString& path);

		/**
		 *	Called each time when a new engine in engine combo box is selected.
		 *	Resets most of the controls and puts engine specific information
		 *	and controls where applicable.
		 */
		void	initEngineSpecific(const PluginInfo* engineInfo);

		/**
		 *	Called once, when the dialog is opened. Handles initialization
		 *	of very basic stuff that's common no matter what the selected
		 *	engine is.
		 */
		void	initPrimary();
};

#endif
