//------------------------------------------------------------------------------
// configurationbasebox.h
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

#ifndef __CONFIGURATION_BASE_BOX_H_
#define __CONFIGURATION_BASE_BOX_H_

#include "global.h"
#include "dptr.h"
#include <QWidget>

/**
 * @ingroup group_pluginapi
 * @brief Base class for configuration pages.
 *
 * Subclassed pages contain actual widgets allowing user input
 * in order to configure program's modules. Specific settings are
 * read and saved through virtual methods: saveSettings() and readSettings().
 *
 * Plugins should rather inherit from EngineConfigurationBaseBox
 * as this provides some default, common behavior, like executable path or
 * custom program parameters configuration.
 */
class MAIN_EXPORT ConfigurationBaseBox : public QWidget
{
	Q_OBJECT;

	public:
		ConfigurationBaseBox(QWidget* parent = NULL);
		virtual ~ConfigurationBaseBox();

		/**
		 * @brief Does this page allow to save the new settings?
		 *
		 * @see setAllowSave()
		 */
		bool allowSave();
		/**
		 * @brief true if settings for this page have already been loaded
		 *        at least once.
		 */
		bool areSettingsAlreadyRead();

		/**
		 * @brief Reimplement this to return a displayable icon for the
		 *        ConfigurationBaseBox.
		 *
		 * If there is no QIcon associated with this box just return
		 * a QIcon object with argument-less constructor.
		 */
		virtual QIcon icon() const = 0;

		/**
		 * @brief Reimplement this to return a list-displayable name for this
		 *        ConfigurationBaseBox.
		 */
		virtual QString name() const = 0;

		/**
		 * @brief Change whether settings on this page can be stored in
		 *        persisting configuration.
		 */
		void setAllowSave(bool b);
		/**
		 * @brief Read configuration from persistence to page contents.
		 */
		void read();
		/**
		 * @brief Save configuration from page contents to persistence.
		 *
		 * @return true if save completes successfully, false on error.
		 */
		bool save();
		/**
		 * @brief Page title, by default returns the same string as name().
		 */
		virtual QString title() const;

	signals:
		/**
		 * @brief Emit to tell Doomseeker to redraw certain widgets.
		 *
		 * This will send a request to Doomseeker to redraw some graphics.
		 * This should be emitted if settings on current page change
		 * program's appearance so that program can redraw affected widgets.
		 */
		void appearanceChanged();

	protected:
		/**
		 * @brief Reimplement this to read settings from config into widgets.
		 */
		virtual void readSettings()=0;

		/**
		 * @brief Reimplement this to write settings to config from widgets.
		 */
		virtual void saveSettings()=0;

	private:
		DPtr<ConfigurationBaseBox> d;
};

#endif
