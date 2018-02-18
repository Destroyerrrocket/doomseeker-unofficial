//------------------------------------------------------------------------------
// engineconfigpage.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 Braden "Blzut3" Obrzut <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __ENGINECONFIGBASE_H__
#define __ENGINECONFIGBASE_H__

#include "gui/configuration/configpage.h"
#include "dptr.h"

class EnginePlugin;
class IniSection;
class QLineEdit;

/**
 * @ingroup group_pluginapi
 * @brief Base for configuration pages for plugins; provides some default
 *        behavior.
 *
 * An extension of ConfigPage. This class is already prepared to
 * support most common and basic settings which include paths to the game's
 * client and server executables, custom parameters and master server address.
 * Plugins can reimplement this widget to benefit from this default behavior
 * and also extend the page with additional widgets.
 *
 * New subclassed instances of this configuration page can be created and
 * returned in a reimplementation of the EnginePlugin::configuration() method.
 */
class MAIN_EXPORT EngineConfigPage : public ConfigPage
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
		EngineConfigPage(EnginePlugin *plugin, IniSection &cfg, QWidget *parent=NULL);
		virtual ~EngineConfigPage();

		QIcon icon() const;
		QString name() const;
		/**
		 * @brief Parent plugin handled by this page.
		 */
		const EnginePlugin *plugin() const;
		QString title() const;
		virtual Validation validate();

	protected:
		/**
		 * @brief Add a new, custom widget below the standard ones.
		 */
		void addWidget(QWidget *widget);
		void readSettings();
		void saveSettings();

	private:
		DPtr<EngineConfigPage> d;
		friend class PrivData<EngineConfigPage>;

#if __GNUC__ == 4 && __GNUC_MINOR__ == 0
	// [QT_VERSION] Remove me when Qt4 is dropped since there won't be a real reason to support GCC 4.0
	// which doesn't support the TR1 nested class visibility change.
	public:
#endif
		QStringList collectKnownGameFilePaths() const;
#if __GNUC__ == 4 && __GNUC_MINOR__ == 0
	private:
#endif
		void makeFileBrowsers();
		QString currentCustomParameters() const;
		void removeStoredCustomParametersFromConfig(const QString &parameters);
		void removeStoredCustomParametersFromWidget(const QString &parameters);
		void showError(const QString &error);

	private slots:
		void autoFindNeighbouringPaths();
		void removeCurrentCustomParametersFromStorage();
		void saveCustomParameters();
		void showFindFailError();
		void updateCustomParametersSaveState();
};

#endif /* __ENGINECONFIGBASE_H__ */
