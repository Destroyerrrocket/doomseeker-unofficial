//------------------------------------------------------------------------------
// createserverdialogpage.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_GUI_WIDGETS_CREATESERVERDIALOGPAGE_H
#define DOOMSEEKER_GUI_WIDGETS_CREATESERVERDIALOGPAGE_H

#include "global.h"
#include <QWidget>

class CreateServerDialog;
class Ini;

/**
 * @ingroup group_pluginapi
 * @brief Base class to be used by plugins to define custom pages in Create
 *        Server dialog.
 */
class MAIN_EXPORT CreateServerDialogPage : public QWidget
{
	public:
		CreateServerDialogPage(CreateServerDialog* pParentDialog, const QString& name);
		virtual ~CreateServerDialogPage();

		/**
		 * @brief Generates game run parameters basing on the page's contents.
		 *
		 * These parameters are passed as arguments to the executable when
		 * the game process is started.
		 */
		virtual QStringList generateGameRunParameters() = 0;

		const QString& name() const;

		/**
		 * @brief Loads variables that are stored in the config into the GUI.
		 *
		 * This config is saved by Doomseeker when it calls saveConfig()
		 * method.

		 * @return Return true on success.
		 */
		virtual bool loadConfig(Ini& ini) = 0;

		/**
		 * @brief Saves variables defined by this page to a config.
		 *
		 * This config is loaded by Doomseeker when it calls loadConfig()
		 * method.
		 *
		 * @return Return true on success.
		 */
		virtual bool saveConfig(Ini& ini) = 0;

		/**
		 * @brief Validates contents of the page before the saveConfig().
		 *
		 * Should return true on validation success or false otherwise. This may
		 * be used to modify the page contents, appearance or behavior.
		 * Validation is run before the game is run. No validation is performed
		 * before saveConfig() or after loadConfig().
		 *
		 * Pages must take care of displaying their own error messages.
		 *
		 * Default behavior assumes that no validation is needed and always
		 * returns true.
		 *
		 * @return Return true on validation success or false on failure.
		 */
		virtual bool validate();

	protected:
		QDialog* parentDialog();

	private:
		class PrivData;
		PrivData *d;
};

#endif
