//------------------------------------------------------------------------------
// dmflagspanel.cpp
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
#include "dmflagspanel.h"

#include "ini/ini.h"
#include "plugins/engineplugin.h"
#include "serverapi/gamecreateparams.h"
#include "serverapi/serverstructs.h"

class DMFlagsPanel::DMFlagsTabWidget
{
public:
	QWidget* widget;
	DMFlagsSection section;

	/**
	 * Check boxes in the same order the flags are stored in the plugin.
	 */
	QList<QCheckBox*> checkBoxes;
};


class DMFlagsPanel::PrivData
{
public:
	QList<DMFlagsTabWidget*> dmFlagsTabs;
};


DMFlagsPanel::DMFlagsPanel(QWidget *parent)
: QWidget(parent)
{
	setupUi(this);
	d = new PrivData();
}

DMFlagsPanel::~DMFlagsPanel()
{
	removeDMFlagsTabs();
	delete d;
}

QList<DMFlagsSection> DMFlagsPanel::dmFlags() const
{
	QList<DMFlagsSection> result;
	foreach(const DMFlagsTabWidget* p, d->dmFlagsTabs)
	{
		DMFlagsSection sec(p->section.name());
		for (int i = 0; i < p->section.count(); ++i)
		{
			if (p->checkBoxes[i]->isChecked())
			{
				sec.add(p->section[i]);
			}
		}
		result << sec;
	}
	return result;
}

bool DMFlagsPanel::initDMFlagsTabs(const EnginePlugin *engine)
{
	removeDMFlagsTabs();

	if (engine->data()->createDMFlagsPagesAutomatic)
	{
		const QList<DMFlagsSection> &dmFlagsSections = engine->data()->allDMFlags;
		if (dmFlagsSections.empty())
		{
			return false; // Nothing to do
		}

		for (int i = 0; i < dmFlagsSections.count(); ++i)
		{
			DMFlagsTabWidget* dmftw = new DMFlagsTabWidget();

			QWidget* flagsTab = new QWidget(this);
			dmftw->widget = flagsTab;
			dmftw->section = dmFlagsSections[i];

			QHBoxLayout* hLayout = new QHBoxLayout(flagsTab);

			QVBoxLayout* layout = NULL;
			for (int j = 0; j < dmFlagsSections[i].count(); ++j)
			{
				if ((j % 16) == 0)
				{
					if (layout != NULL)
					{
						layout->addStretch();
					}

					layout = new QVBoxLayout();
					hLayout->addLayout(layout);
				}

				QCheckBox* checkBox = new QCheckBox();
				checkBox->setText(dmFlagsSections[i][j].name());
				dmftw->checkBoxes << checkBox;
				layout->addWidget(checkBox);
			}

			if (layout != NULL)
			{
				layout->addStretch();
			}

			d->dmFlagsTabs << dmftw;
			tabWidget->addTab(flagsTab, dmFlagsSections[i].name());
		}
		return true;
	}
	return false;
}

void DMFlagsPanel::removeDMFlagsTabs()
{
	foreach (DMFlagsTabWidget* flags, d->dmFlagsTabs)
	{
		int index = tabWidget->indexOf(flags->widget);
		tabWidget->removeTab(index);
		delete flags->widget;
		delete flags;
	}

	d->dmFlagsTabs.clear();
}

void DMFlagsPanel::fillInParams(GameCreateParams &params)
{
	params.dmFlags() = dmFlags();
}

void DMFlagsPanel::loadConfig(Ini &config)
{
	IniSection dmflags = config.section("DMFlags");
	foreach(DMFlagsTabWidget* p, d->dmFlagsTabs)
	{
		for (int i = 0; i < p->section.count(); ++i)
		{
			p->checkBoxes[i]->setChecked(dmflags[p->section.name() + "/" + p->section[i].name()]);
		}
	}
}

void DMFlagsPanel::saveConfig(Ini &config)
{
	IniSection dmflags = config.createSection("DMFlags");
	foreach(DMFlagsTabWidget* p, d->dmFlagsTabs)
	{
		for (int i = 0; i < p->section.count(); ++i)
		{
			dmflags[p->section.name() + "/" + p->section[i].name()] = p->checkBoxes[i]->isChecked();
		}
	}
}
