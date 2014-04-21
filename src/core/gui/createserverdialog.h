//------------------------------------------------------------------------------
// createserverdialog.h
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
// Copyright (C) 2009-2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_GUI_CREATESERVERDIALOG_H
#define DOOMSEEKER_GUI_CREATESERVERDIALOG_H

#include "ui_createserverdialog.h"
#include "serverapi/serverstructs.h"
#include <QCheckBox>
#include <QDialog>

class CreateServerDialogPage;
class EnginePlugin;
class GameCreateParams;
class Message;
class Server;

/**
 * @ingroup group_pluginapi
 * @brief Dialog window allowing user to host a game.
 */
class MAIN_EXPORT CreateServerDialog : public QDialog, private Ui::CreateServerDialog
{
	Q_OBJECT

	public:
		CreateServerDialog(QWidget* parent = NULL);
		virtual ~CreateServerDialog();

		bool commandLineArguments(QString &executable, QStringList &args);
		void makeSetupServerDialog(const EnginePlugin *plugin);

	private slots:
		void btnAddMapToMaplistClicked();
		void btnAddPwadClicked();
		void btnBrowseExecutableClicked();
		void btnCommandLineClicked();
		void btnClearPwadListClicked();
		void btnDefaultExecutableClicked();
		void btnIwadBrowseClicked();
		void btnLoadClicked();
		void btnPlayOfflineClicked();
		void btnRemoveMapFromMaplistClicked();
		void btnRemovePwadClicked();
		void btnSaveClicked();
		void btnStartServerClicked();
		void cboEngineSelected(int index);
		void cboGamemodeSelected(int index);
		void firstLoadConfigTimer();
		void focusChanged(QWidget* oldW, QWidget* newW);

		/**
		 * @brief Files drag'n'drop on WADs list view.
		 */
		void lstAdditionalFilesPathDnd(const QString& path);

	private:
		class DMFlagsTabWidget
		{
			public:
				QWidget* widget;
				DMFlagsSection section;

				/**
 				 * Check boxes in the same order the flags are stored in the plugin.
 				 */
				QList<QCheckBox*> checkBoxes;
		};

		class GameLimitWidget
		{
			public:
				QWidget* label;
				QSpinBox* spinBox;
				GameCVar limit;
		};

		static const QString TEMP_SERVER_CONFIG_FILENAME;

		class PrivData;
		PrivData *d;

		/**
		 * Adds IWAD path to the IWAD ComboBox.
		 * If the path already exists no new items will be added but this path
		 * will be selected.
		 */
		void addIwad(const QString& path);
		void addMapToMaplist(const QString& map);
		void addWadPath(const QString& path);

		/**
		 * Sets host information for both server and hi objects. Both
		 * parameters obtain new information after this method is called.
		 * @return false if fail.
		 */
		bool createHostInfo(GameCreateParams& params, bool offline);

		void initDMFlagsTabs();

		/**
		 *	Called each time when a new engine in engine combo box is selected.
		 *	Resets most of the controls and puts engine specific information
		 *	and controls where applicable.
		 */
		void initEngineSpecific(EnginePlugin* engineInfo);

		/**
		 * @brief Loads pages specific for the given engine.
		 *
		 * @see CreateServerDialogPage
		 */
		void initEngineSpecificPages(EnginePlugin* engineInfo);

		void initGamemodeSpecific(const GameMode& gameMode);

		void initInfoAndPassword();

		/**
		 * Called once, when the dialog is opened. Handles initialization
		 * of very basic stuff that's common no matter what the selected
		 * engine is.
		 */
		void initPrimary();
		void initRules();

		bool loadConfig(const QString& filename);
		QString pathToClientExe(Server* server, Message& message);
		QString pathToOfflineExe(Message& message);
		QString pathToServerExe(Message& message);
		void removeDMFlagsTabs();
		void removeLimitWidgets();
		void runGame(bool offline);
		bool saveConfig(const QString& filename);
		bool setEngine(const QString &engineName);
};

#endif
