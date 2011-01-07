//------------------------------------------------------------------------------
// serverfilterdock.cpp
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "serverfilterdock.h"

#include "gui/entity/serverlistfilterinfo.h"

ServerFilterDock::ServerFilterDock(QWidget* pParent)
: QDockWidget(pParent) 
{
	setupUi(this);
	doConnections();
	
	this->toggleViewAction()->setIcon(QIcon(":/icons/filter.png"));
	
	// Empty item means no filter.
	cboGameMode->addItem("");
	
	toggleViewAction()->setText(tr("Server &Filter"));
	toggleViewAction()->setShortcut(tr("CTRL+F"));	
}

void ServerFilterDock::addGameModeToComboBox(const QString& gameMode)
{
	QString name = gameMode.trimmed();
	
	// No duplicates allowed.
	if (cboGameMode->findText(name, Qt::MatchFixedString) < 0)
	{
		// Make sure combobox contents are sorted.
		for (int i = 0; i < cboGameMode->count(); ++i)
		{
			if (name < cboGameMode->itemText(i))
			{
				cboGameMode->insertItem(i, name);
				return;
			}
		}
		
		// The above routine didn't return.
		// This item belongs to the end of the list.
		cboGameMode->addItem(name);
	}
}

void ServerFilterDock::btnClearClicked()
{
	this->setFilterInfo(ServerListFilterInfo());
}

void ServerFilterDock::clearGameModes()
{
	cboGameMode->clear();
	cboGameMode->addItem("");
	
	emit filterUpdated(filterInfo());
}

QLineEdit *ServerFilterDock::createQuickSearch() const
{
	QLineEdit *qs = new QLineEdit();
	qs->setText(leServerName->text());

	connect(leServerName, SIGNAL( textChanged(const QString &) ), qs, SLOT( setText(const QString &) ));
	connect(qs, SIGNAL( textChanged(const QString &) ), leServerName, SLOT( setText(const QString &) ));

	return qs;
}

void ServerFilterDock::doConnections()
{
	this->connect(this, SIGNAL( visibilityChanged(bool) ),
		SLOT( thisVisibilityChanged(bool) ) );
		
	this->connect(btnClear, SIGNAL( clicked() ),
		SLOT( btnClearClicked() ) );
	
	this->connect(cbShowEmpty, SIGNAL( clicked() ),
		SLOT( emitUpdated() ) );	
	
	this->connect(cbShowFull, SIGNAL( clicked() ),
		SLOT( emitUpdated() ) );	
		
	this->connect(cbShowOnlyValid, SIGNAL( clicked() ),
		SLOT( emitUpdated() ) );	
		
	this->connect(cboGameMode, SIGNAL( currentIndexChanged(const QString &) ),
		SLOT( emitUpdated(const QString&) ) );	
	
	this->connect(leServerName, SIGNAL( textChanged(const QString &) ),
		SLOT( emitUpdated(const QString&) ) );	
		
	this->connect(leWads, SIGNAL( textEdited(const QString &) ),
		SLOT( emitUpdated(const QString&) ) );	
		
	this->connect(spinMaxPing, SIGNAL( editingFinished() ),
		SLOT( emitUpdated() ) );		
}

void ServerFilterDock::emitUpdated()
{
	emit filterUpdated(filterInfo());
}

void ServerFilterDock::emitUpdated(const QString& dummy)
{
	emit filterUpdated(filterInfo());
}

ServerListFilterInfo ServerFilterDock::filterInfo() const
{
	ServerListFilterInfo filterInfo;
	
	filterInfo.bShowEmpty = cbShowEmpty->isChecked();
	filterInfo.bShowFull = cbShowFull->isChecked();
	filterInfo.bShowOnlyValid = cbShowOnlyValid->isChecked();
	filterInfo.gameMode = cboGameMode->currentText();
	filterInfo.maxPing = spinMaxPing->value();
	filterInfo.serverName = leServerName->text();
	filterInfo.wads = leWads->text().split(";");
	
	return filterInfo;
}

void ServerFilterDock::setFilterInfo(const ServerListFilterInfo& filterInfo)
{
	cbShowEmpty->setChecked(filterInfo.bShowEmpty);
	cbShowFull->setChecked(filterInfo.bShowFull);
	cbShowOnlyValid->setChecked(filterInfo.bShowOnlyValid);
	
	QString gameMode = filterInfo.gameMode.trimmed();
	
	addGameModeToComboBox(gameMode);
	cboGameMode->setCurrentIndex(cboGameMode->findText(gameMode));
	
	spinMaxPing->setValue(filterInfo.maxPing);
	leServerName->setText(filterInfo.serverName.trimmed());
	leWads->setText(filterInfo.wads.join(";"));
	
	emitUpdated();
}

void ServerFilterDock::thisVisibilityChanged(bool bVisible)
{
	leServerName->setFocus();
}
