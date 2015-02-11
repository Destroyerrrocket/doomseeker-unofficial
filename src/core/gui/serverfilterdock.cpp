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
#include "ui_serverfilterdock.h"

#include "configuration/doomseekerconfig.h"
#include "gui/entity/serverlistfilterinfo.h"

DClass<ServerFilterDock> : public Ui::ServerFilterDock
{
public:
	/**
	* @brief Quick Search widget that is actually located outside the
	* ServerFilterDock.
	*
	* ServerFilterDock needs to keep track of this widget in order to
	* update the changing value appropriately.
	*/
	QLineEdit* leQuickSearch;
	/**
		* Guard used to prevent multiple signals being generated while loading
		* a filter with setFilterInfo.
		*/
	bool bDisableUpdate;
};

DPointered(ServerFilterDock)

ServerFilterDock::ServerFilterDock(QWidget* pParent)
: QDockWidget(pParent)
{
	d->setupUi(this);
	d->leQuickSearch = NULL;
	d->bDisableUpdate = false;
	d->cbGroupServersWithPlayersAtTop->setChecked(gConfig.doomseeker.bGroupServersWithPlayersAtTheTopOfTheList);

	toggleViewAction()->setIcon(QIcon(":/icons/filter.png"));

	toggleViewAction()->setText(tr("Server &Filter"));
	toggleViewAction()->setShortcut(tr("CTRL+F"));
}

ServerFilterDock::~ServerFilterDock()
{
}

void ServerFilterDock::addGameModeToComboBox(const QString& gameMode)
{
	addSortedNonDuplicate(d->cboGameMode, gameMode.trimmed());
	addSortedNonDuplicate(d->cboExcludeGameMode, gameMode.trimmed());
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
	if (d->leQuickSearch == NULL)
	{
		QLineEdit *qs = new QLineEdit();
		qs->setText(d->leServerName->text());

		connect(d->leServerName, SIGNAL( textEdited(const QString &) ), qs, SLOT( setText(const QString &) ));
		connect(qs, SIGNAL( textEdited(const QString &) ), d->leServerName, SLOT( setText(const QString &) ));
		connect(qs, SIGNAL(textEdited(QString)), SLOT(enableFilter()));

		d->leQuickSearch = qs;
	}

	return d->leQuickSearch;
}

void ServerFilterDock::emitUpdated()
{
	if(d->bDisableUpdate)
		return;

	emit filterUpdated(filterInfo());
}

void ServerFilterDock::enableFilter()
{
	d->cbFilteringEnabled->setChecked(true);
	emitUpdated();
}

ServerListFilterInfo ServerFilterDock::filterInfo() const
{
	ServerListFilterInfo filterInfo;

	filterInfo.bEnabled = d->cbFilteringEnabled->isChecked();
	filterInfo.bShowEmpty = d->cbShowEmpty->isChecked();
	filterInfo.bShowFull = d->cbShowFull->isChecked();
	filterInfo.bShowOnlyValid = d->cbShowOnlyValid->isChecked();
	filterInfo.gameModes = d->cboGameMode->selectedItemTexts();
	filterInfo.gameModesExcluded = d->cboExcludeGameMode->selectedItemTexts();
	filterInfo.maxPing = d->spinMaxPing->value();
	filterInfo.serverName = d->leServerName->text();
	filterInfo.wads = d->leWads->text().trimmed().split(",", QString::SkipEmptyParts);
	filterInfo.wadsExcluded = d->leExcludeWads->text().trimmed().split(",", QString::SkipEmptyParts);

	return filterInfo;
}

void ServerFilterDock::onServerGroupingChange()
{
	gConfig.doomseeker.bGroupServersWithPlayersAtTheTopOfTheList = d->cbGroupServersWithPlayersAtTop->isChecked();
	emit nonEmptyServerGroupingAtTopToggled(d->cbGroupServersWithPlayersAtTop->isChecked());
}

void ServerFilterDock::setFilterInfo(const ServerListFilterInfo& filterInfo)
{
	d->bDisableUpdate = true;

	d->cbFilteringEnabled->setChecked(filterInfo.bEnabled);
	d->cbShowEmpty->setChecked(filterInfo.bShowEmpty);
	d->cbShowFull->setChecked(filterInfo.bShowFull);
	d->cbShowOnlyValid->setChecked(filterInfo.bShowOnlyValid);

	foreach (const QString& gameMode, filterInfo.gameModes)
	{
		addGameModeToComboBox(gameMode);
	}
	d->cboGameMode->setSelectedTexts(filterInfo.gameModes);

	foreach (const QString& gameMode, filterInfo.gameModesExcluded)
	{
		addGameModeToComboBox(gameMode);
	}
	d->cboExcludeGameMode->setSelectedTexts(filterInfo.gameModesExcluded);

	d->spinMaxPing->setValue(filterInfo.maxPing);
	if (d->leQuickSearch != NULL)
	{
		d->leQuickSearch->setText(filterInfo.serverName.trimmed());
	}

	d->leServerName->setText(filterInfo.serverName.trimmed());
	d->leWads->setText(filterInfo.wads.join(",").trimmed());
	d->leExcludeWads->setText(filterInfo.wadsExcluded.join(",").trimmed());

	d->bDisableUpdate = false;
	emitUpdated();
}
