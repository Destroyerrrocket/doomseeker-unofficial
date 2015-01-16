//------------------------------------------------------------------------------
// maplistpanel.cpp
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
#include "maplistpanel.h"
#include "ui_maplistpanel.h"

#include "ini/ini.h"
#include "plugins/engineplugin.h"
#include "serverapi/gamecreateparams.h"
#include "commongui.h"
#include <QStandardItemModel>

class MapListPanel::PrivData : public Ui::MapListPanel
{
};

MapListPanel::MapListPanel(QWidget *parent)
:QWidget(parent)
{
	d = new PrivData;
	d->setupUi(this);
	d->lstMaplist->setModel(new QStandardItemModel(this));
	this->connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)),
		SLOT(onFocusChanged(QWidget*, QWidget*)));
}

MapListPanel::~MapListPanel()
{
	delete d;
}

void MapListPanel::addMapFromEditBoxToList()
{
	addMapToMaplist(d->leMapname->text().trimmed());
}

void MapListPanel::addMapToMaplist(const QString &map)
{
	if (map.isEmpty())
	{
		return;
	}
	QStandardItemModel* model = static_cast<QStandardItemModel*>(d->lstMaplist->model());
	QStandardItem* it = new QStandardItem(map);
	it->setDragEnabled(true);
	it->setDropEnabled(false);
	model->appendRow(it);
}

void MapListPanel::onFocusChanged(QWidget* old, QWidget* now)
{
	if (now == d->leMapname)
	{
		d->btnAddMapToMaplist->setDefault(true);
	}
	else if (old == d->leMapname)
	{
		d->btnAddMapToMaplist->setDefault(false);
	}
}

void MapListPanel::removeSelectedFromList()
{
	const bool bSelectNextLowest = true;
	CommonGUI::removeSelectedRowsFromStandardItemView(d->lstMaplist, bSelectNextLowest);
}

void MapListPanel::fillInParams(GameCreateParams &params)
{
	params.setMapList(CommonGUI::listViewStandardItemsToStringList(d->lstMaplist));
	params.setRandomMapRotation(d->cbRandomMapRotation->isChecked());
}

void MapListPanel::loadConfig(Ini &config)
{
	IniSection section = config.section("Rules");
	QStringList stringList = section["maplist"].valueString().split(";");
	QAbstractItemModel *model = d->lstMaplist->model();
	model->removeRows(0, model->rowCount());
	foreach(QString s, stringList)
	{
		addMapToMaplist(s);
	}
	d->cbRandomMapRotation->setChecked(section["randomMapRotation"]);
}

void MapListPanel::saveConfig(Ini &config)
{
	IniSection section = config.section("Rules");
	QStringList stringList = CommonGUI::listViewStandardItemsToStringList(d->lstMaplist);
	section["maplist"] = stringList.join(";");
	section["randomMapRotation"] = d->cbRandomMapRotation->isChecked();
}

void MapListPanel::setupForEngine(const EnginePlugin *engine)
{
	d->cbRandomMapRotation->setVisible(engine->data()->supportsRandomMapRotation);
}
