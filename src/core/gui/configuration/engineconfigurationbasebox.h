//------------------------------------------------------------------------------
// engineconfigurationbasebox.h
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __ENGINECONFIGBASE_H__
#define __ENGINECONFIGBASE_H__

#include "gui/configuration/configurationbasebox.h"
#include "ui_engineconfigurationbasebox.h"

class EnginePlugin;

/**
 * @ingroup group_pluginapi
 * @brief Base for configuration pages for plugins; provides some default
 *        behavior.
 *
 * An extension of ConfigurationBaseBox. This class is already prepared to
 * support most common and basic settings which include paths to the game's
 * client and server executables, custom parameters and master server address.
 * Plugins can reimplement this widget to benefit from this default behavior
 * and also extend the page with additional widgets.
 *
 * New subclassed instances of this configuration page can be created and
 * returned in a reimplementation of the EnginePlugin::configuration() method.
 */
class MAIN_EXPORT EngineConfigurationBaseBox : public ConfigurationBaseBox, private Ui::EngineConfigurationBaseBox
{
	Q_OBJECT

	public:
		/**
		 * @param plugin
		 *     Parent plugin handled by this page.
		 * @param cfg
		 *     IniSection which handles configuration for this plugin. Usually
		 *     this should be set to whatever EnginePlugin::data()->pConfig
		 *     points to.
		 * @param parent
		 *     Parent widget, most likely configuration dialog box.
		 */
		EngineConfigurationBaseBox(const EnginePlugin *plugin, IniSection &cfg, QWidget *parent=NULL);
		virtual ~EngineConfigurationBaseBox();

		QIcon icon() const;
		QString name() const;
		void readSettings();
		/**
		 * @brief Parent plugin handled by this page.
		 */
		const EnginePlugin *plugin() const;
		QString title() const;

	protected:
		/**
		 * @brief Add a new, custom widget below the standard ones.
		 */
		void addWidget(QWidget *widget);
		/**
		 * @brief Open file browse dialog and update "input" when successful.
		 *
		 * @param input
		 *     A QLineEdit widget which contents are changed when user
		 *     picks a valid file.
		 * @param type
		       Name of executable type, displayed in browser's title.
		 */
		void browseForBinary(QLineEdit *input, const QString &type);
		void makeClientOnly();
		void saveSettings();

	protected slots:
		void btnBrowseClientBinaryClicked();
		void btnBrowseServerBinaryClicked();

	private:
		class PrivData;
		PrivData *d;
};

#endif /* __ENGINECONFIGBASE_H__ */
