//------------------------------------------------------------------------------
// engineconfigurationbasebox.cpp
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

#include "engineconfigurationbasebox.h"
#include "plugins/engineplugin.h"

#include <QFileDialog>

class EngineConfigurationBaseBox::PrivData
{
	public:
		IniSection *config;
		const EnginePlugin *plugin;
		bool clientOnly;

		QStringList readStoredCustomParameters() const
		{
			return config->value("StoredCustomParameters").toStringList();
		}

		void saveStoredCustomParameters(const QStringList &params)
		{
			config->setValue("StoredCustomParameters", params);
		}

		bool existsInStoredCustomParameters(const QString &text) const
		{
			return readStoredCustomParameters().contains(text);
		}
};

EngineConfigurationBaseBox::EngineConfigurationBaseBox(const EnginePlugin *plugin, IniSection &cfg, QWidget *parent) 
: ConfigurationBaseBox(parent)
{
	d = new PrivData();
	d->plugin = plugin;
	d->config = &cfg;
	setupUi(this);

	if(plugin->data()->clientOnly)
		makeClientOnly();

	if(!plugin->data()->hasMasterServer)
		masterAddressBox->hide();

	connect(btnBrowseClientBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseClientBinaryClicked() ));
	connect(btnBrowseServerBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseServerBinaryClicked() ));
}

EngineConfigurationBaseBox::~EngineConfigurationBaseBox()
{
	delete d;
}

void EngineConfigurationBaseBox::addWidget(QWidget *widget)
{
	layout()->removeItem(verticalSpacer);
	layout()->addWidget(widget);
	layout()->addItem(verticalSpacer);
}

void EngineConfigurationBaseBox::browseForBinary(QLineEdit *input, const QString &type)
{
	QString filter;
#if defined(Q_OS_WIN32)
	filter = tr("Binary files (*.exe);;Any files (*)");
#else
	// Other platforms do not have an extension for their binary files.
	filter = tr("Any files(*)");
#endif
	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose ") + d->plugin->data()->name + " " + type, QString(), filter);
	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		input->setText(strFilepath);
}

void EngineConfigurationBaseBox::btnBrowseClientBinaryClicked()
{
	browseForBinary(leClientBinaryPath, tr("client binary"));

}

void EngineConfigurationBaseBox::btnBrowseServerBinaryClicked()
{
	browseForBinary(leServerBinaryPath, tr("server binary"));
}

QString EngineConfigurationBaseBox::currentCustomParameters() const
{
	return cboCustomParameters->currentText().trimmed();
}

QIcon EngineConfigurationBaseBox::icon() const
{
	return d->plugin->icon();
}

void EngineConfigurationBaseBox::makeClientOnly()
{
	d->clientOnly = true;

	lblClientBinary->setText(tr("Path to executable:"));
	serverBinaryBox->hide();
}

QString EngineConfigurationBaseBox::name() const
{
	return d->plugin->data()->name;
}

const EnginePlugin *EngineConfigurationBaseBox::plugin() const
{
	return d->plugin;
}

void EngineConfigurationBaseBox::readSettings()
{
	leClientBinaryPath->setText(d->config->value("BinaryPath").toString());
	cboCustomParameters->clear();
	cboCustomParameters->addItems(d->readStoredCustomParameters());
	cboCustomParameters->setEditText(d->config->value("CustomParameters").toString());
	if(d->plugin->data()->hasMasterServer)
		leMasterserverAddress->setText(d->config->value("Masterserver").toString());
	leServerBinaryPath->setText(d->config->value("ServerBinaryPath").toString());
}

void EngineConfigurationBaseBox::removeCurrentCustomParametersFromStorage()
{
	QString currentParams = currentCustomParameters();
	removeStoredCustomParametersFromConfig(currentParams);
	removeStoredCustomParametersFromWidget(currentParams);
	updateCustomParametersSaveState();
}

void EngineConfigurationBaseBox::removeStoredCustomParametersFromConfig(const QString &parameters)
{
	QStringList storedParameters = d->readStoredCustomParameters();
	storedParameters.removeAll(parameters);
	d->saveStoredCustomParameters(storedParameters);
}

void EngineConfigurationBaseBox::removeStoredCustomParametersFromWidget(const QString &parameters)
{
	int indexInWidget = cboCustomParameters->findText(parameters);
	if (indexInWidget >= 0)
	{
		cboCustomParameters->removeItem(indexInWidget);
		cboCustomParameters->setEditText(parameters);
	}
}

void EngineConfigurationBaseBox::saveCustomParameters()
{
	if (!d->existsInStoredCustomParameters(currentCustomParameters()))
	{
		QStringList parameters = d->readStoredCustomParameters();
		parameters << currentCustomParameters();
		d->saveStoredCustomParameters(parameters);
		cboCustomParameters->addItem(currentCustomParameters());
	}
	updateCustomParametersSaveState();
}

void EngineConfigurationBaseBox::saveSettings()
{
	QString executable;

	executable = leClientBinaryPath->text();
	d->config->setValue("BinaryPath", executable);
	if (!d->clientOnly)
	{
		executable = leServerBinaryPath->text();
	}
	d->config->setValue("ServerBinaryPath", executable);
	d->config->setValue("CustomParameters", currentCustomParameters());
	if (d->plugin->data()->hasMasterServer)
	{
		d->config->setValue("Masterserver", leMasterserverAddress->text());
	}
}

QString EngineConfigurationBaseBox::title() const
{
	return tr("Game - %1").arg(name());
}

void EngineConfigurationBaseBox::updateCustomParametersSaveState()
{
	bool paramExists = d->existsInStoredCustomParameters(currentCustomParameters());
	btnSaveCustomParameters->setVisible(!paramExists);
	btnDeleteCustomParameters->setVisible(paramExists);
}
