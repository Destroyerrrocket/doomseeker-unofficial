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
#include <QCheckBox>
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

	public slots:
		void	accept();

	protected slots:
		void	btnAddMapToMaplistClicked();
		void	btnAddPwadClicked();
		void	btnIwadBrowseClicked();
		void	btnLoadClicked();
		void	btnRemoveMapFromMaplistClicked();
		void	btnRemovePwadClicked();
		void	btnSaveClicked();
		void	cboEngineSelected(int index);
		void	cboGamemodeSelected(int index);

	protected:
		struct DMFlagsTabWidget
		{
			QWidget*				widget;
			const DMFlagsSection*	section;

			/**
			 *	Check boxes in the same order the flags are stored in the plugin.
			 */
			QList<QCheckBox*>		checkBoxes;
		};

		struct GameLimitWidget
		{
			QWidget*	label;
			QSpinBox*	spinBox;
			GameCVar	limit;
		};

		const PluginInfo* 				currentEngine;
		QList<DMFlagsTabWidget*>		dmFlagsTabs;
		QList<GameLimitWidget*>			limitWidgets;
		QList<GameCVar>					gameModifiers;


		/**
		 *	Adds IWAD path to the IWAD ComboBox.
		 *	If the path already exists no new items will be added but this path
		 *	will be selected.
		 */
		void	addIwad(const QString& path);
		void	addMapToMaplist(const QString& map);
		void	addWadPath(const QString& path);

		void	initDMFlagsTabs();

		/**
		 *	Called each time when a new engine in engine combo box is selected.
		 *	Resets most of the controls and puts engine specific information
		 *	and controls where applicable.
		 */
		void	initEngineSpecific(const PluginInfo* engineInfo);

		void	initGamemodeSpecific(const GameMode& gameMode);

		void	initInfoAndPassword();

		/**
		 *	Called once, when the dialog is opened. Handles initialization
		 *	of very basic stuff that's common no matter what the selected
		 *	engine is.
		 */
		void	initPrimary();

		void	initRules();

		void	removeDMFlagsTabs();
		void	removeLimitWidgets();
};

#endif
