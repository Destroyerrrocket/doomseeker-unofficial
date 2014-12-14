//------------------------------------------------------------------------------
// createserverdialogapi.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "createserverdialogapi.h"

#include "gui/createserverdialog.h"
#include <QPointer>
#include <cassert>

class CreateServerDialogApi::PrivData
{
public:
	QPointer<CreateServerDialog> dialog;
};

CreateServerDialogApi::CreateServerDialogApi(QWidget *parent)
{
	d = new PrivData();
	d->dialog = new CreateServerDialog(parent);
}

CreateServerDialogApi::~CreateServerDialogApi()
{
	if (d->dialog)
	{
		delete d->dialog;
	}
	delete d;
}

QDialog *CreateServerDialogApi::dialog() const
{
	return d->dialog;
}

bool CreateServerDialogApi::fillInCommandLineArguments(QString &executable, QStringList &args)
{
	assert(d->dialog);
	return d->dialog->commandLineArguments(executable, args);
}

void CreateServerDialogApi::makeRemoteGameSetup(const EnginePlugin *plugin)
{
	assert(d->dialog);
	d->dialog->makeRemoteGameSetupDialog(plugin);
}
