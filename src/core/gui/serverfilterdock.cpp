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
	leQuickSearch = NULL;

	this->toggleViewAction()->setIcon(QIcon(":/icons/filter.png"));

	toggleViewAction()->setText(tr("Server &Filter"));
	toggleViewAction()->setShortcut(tr("CTRL+F"));
}

void ServerFilterDock::addGameModeToComboBox(const QString& gameMode)
{
	addSortedNonDuplicate(cboGameMode, gameMode.trimmed());
	addSortedNonDuplicate(cboExcludeGameMode, gameMode.trimmed());
}

void ServerFilterDock::addSortedNonDuplicate(QComboBox* comboBox, const QString& text)
{
	if (comboBox->findText(text, Qt::MatchFixedString) < 0)
	{
		// Make sure combobox contents are sorted.
		for (int i = 0; i < comboBox->count(); ++i)
		{
			if (text < comboBox->itemText(i))
			{
				comboBox->insertItem(i, text);
				return;
			}
		}

		// The above routine didn't return.
		// This item belongs to the end of the list.
		comboBox->addItem(text);
	}
}

void ServerFilterDock::clear()
{
	this->setFilterInfo(ServerListFilterInfo());
}

QLineEdit *ServerFilterDock::createQuickSearch()
{
	if (leQuickSearch == NULL)
	{
		QLineEdit *qs = new QLineEdit();
		qs->setText(leServerName->text());

		connect(leServerName, SIGNAL( textEdited(const QString &) ), qs, SLOT( setText(const QString &) ));
		connect(qs, SIGNAL( textEdited(const QString &) ), leServerName, SLOT( setText(const QString &) ));

		leQuickSearch = qs;
	}

	return leQuickSearch;
}

void ServerFilterDock::emitUpdated()
{
	emit filterUpdated(filterInfo());
}

ServerListFilterInfo ServerFilterDock::filterInfo() const
{
	ServerListFilterInfo filterInfo;

	filterInfo.bShowEmpty = cbShowEmpty->isChecked();
	filterInfo.bShowFull = cbShowFull->isChecked();
	filterInfo.bShowOnlyValid = cbShowOnlyValid->isChecked();
	filterInfo.gameModes = cboGameMode->selectedItemTexts();
	filterInfo.gameModesExcluded = cboExcludeGameMode->selectedItemTexts();
	filterInfo.maxPing = spinMaxPing->value();
	filterInfo.serverName = leServerName->text();
	filterInfo.wads = leWads->text().trimmed().split(",", QString::SkipEmptyParts);
	filterInfo.wadsExcluded = leExcludeWads->text().trimmed().split(",", QString::SkipEmptyParts);

	return filterInfo;
}

void ServerFilterDock::setFilterInfo(const ServerListFilterInfo& filterInfo)
{
	cbShowEmpty->setChecked(filterInfo.bShowEmpty);
	cbShowFull->setChecked(filterInfo.bShowFull);
	cbShowOnlyValid->setChecked(filterInfo.bShowOnlyValid);

	foreach (const QString& gameMode, filterInfo.gameModes)
	{
		addGameModeToComboBox(gameMode);
	}
	cboGameMode->setSelectedTexts(filterInfo.gameModes);

	foreach (const QString& gameMode, filterInfo.gameModesExcluded)
	{
		addGameModeToComboBox(gameMode);
	}
	cboExcludeGameMode->setSelectedTexts(filterInfo.gameModesExcluded);

	spinMaxPing->setValue(filterInfo.maxPing);
	if (leQuickSearch != NULL)
	{
		leQuickSearch->setText(filterInfo.serverName.trimmed());
	}

	leServerName->setText(filterInfo.serverName.trimmed());
	leWads->setText(filterInfo.wads.join(",").trimmed());
	leExcludeWads->setText(filterInfo.wadsExcluded.join(",").trimmed());

	emitUpdated();
}
