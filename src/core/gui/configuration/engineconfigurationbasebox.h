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

class MAIN_EXPORT EngineConfigurationBaseBox : public ConfigurationBaseBox, private Ui::EngineConfigurationBaseBox
{
		Q_OBJECT

	public:
		EngineConfigurationBaseBox(const EnginePlugin *plugin, IniSection &cfg, QWidget *parent=NULL);

		QIcon				icon() const;
		QString				name() const;
		void				readSettings();

	protected:
		void				addWidget(QWidget *widget);
		void				browseForBinary(QLineEdit *input, const QString &type);
		void				makeClientOnly();
		void				saveSettings();

	protected slots:
		void				btnBrowseClientBinaryClicked();
		void				btnBrowseServerBinaryClicked();

	private:
		IniSection&			config;
		const EnginePlugin*	plugin;
		bool				clientOnly;
};

#endif /* __ENGINECONFIGBASE_H__ */
