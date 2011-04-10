//------------------------------------------------------------------------------
// demomanager.h
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
// Copyright (C) 2011 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "demomanager.h"
#include "main.h"
#include "pathfinder.h"
#include "sdeapi/pluginloader.hpp"
#include "serverapi/messages.h"
#include "serverapi/server.h"
#include "serverapi/gamerunner.h"
#include "serverapi/gamerunnerstructs.h"

#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>

DemoManagerDlg::DemoManagerDlg() : selectedDemo(NULL)
{
	setupUi(this);

	// Add a play button
	QPushButton *playButton = buttonBox->addButton("Play", QDialogButtonBox::ActionRole);
	playButton->setIcon(QIcon(":/icons/media-playback-start.png"));

	demoModel = new QStandardItemModel();
	adjustDemoList();

	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( performAction(QAbstractButton *) ));
	connect(demoList->selectionModel(), SIGNAL( currentChanged(const QModelIndex &, const QModelIndex &) ), this, SLOT( updatePreview(const QModelIndex &) ));
}

void DemoManagerDlg::adjustDemoList()
{
	// Populate the demos list (*.cld* because some ports add .lmp extension automatically)
	// In order to index the demos we'll convert the dates to integers by calculating the days until today.
	QDate today = QDate::currentDate();
	QTime referenceTime(23, 59, 59);
	QDir demosDirectory(Main::dataPaths->demosDirectoryPath());
	QStringList demos = demosDirectory.entryList(QStringList("*.cld*"), QDir::Files);
	typedef QMap<int, Demo> DemoMap;
	QMap<int, DemoMap> demoMap;
	foreach(const QString &demoName, demos)
	{
		QStringList demoData = demoName.left(demoName.lastIndexOf(".cld")).split("_");
		if(demoData.size() < 4) // Should have at least 4 elements port, date, time, iwad[, pwads]
			continue;

		QDate date = QDate::fromString(demoData[1], "dd.MM.yyyy");
		QTime time = QTime::fromString(demoData[2], "hh.mm.ss");
		Demo demo;
		demo.filename = demoName;
		demo.port = demoData[0];
		demo.time = QDateTime(date, time);
		demo.wads = demoData.mid(3);

		demoMap[date.daysTo(today)][time.secsTo(referenceTime)] = demo;
	}

	// Convert to a model
	demoModel->clear();
	demoTree.clear();
	foreach(const DemoMap &demoDate, demoMap)
	{
		QStandardItem *item = new QStandardItem(demoDate.begin().value().time.toString("ddd. MMM d, yyyy"));
		QList<Demo> demoDateList;
		foreach(const Demo &demo, demoDate)
		{
			demoDateList << demo;
			item->appendRow(new QStandardItem(demo.time.toString("hh:mm:ss")));
		}
		demoTree << demoDateList;
		demoModel->appendRow(item);
	}
	demoList->setModel(demoModel);
}

void DemoManagerDlg::performAction(QAbstractButton *button)
{
	if(button == buttonBox->button(QDialogButtonBox::Close))
		reject();
	else if(button == buttonBox->button(QDialogButtonBox::Save))
	{
		if(selectedDemo == NULL)
			return;

		QFileDialog saveDialog(this);
		saveDialog.setAcceptMode(QFileDialog::AcceptSave);
		saveDialog.selectFile(selectedDemo->filename);
		if(saveDialog.exec() == QDialog::Accepted)
		{
			// Copy the demo to the new location.
			if(!QFile::copy(Main::dataPaths->demosDirectoryPath() + QDir::separator() + selectedDemo->filename, saveDialog.selectedFiles().first()))
				QMessageBox::critical(this, tr("Unable to save"), tr("Could not write to the specified location."));
		}
	}
	else if(button == buttonBox->button(QDialogButtonBox::Discard))
	{
		if(QMessageBox::question(this, tr("Delete demo?"), tr("Are you sure you want to delete the selected demo?"), QMessageBox::Yes|QMessageBox::Cancel) == QMessageBox::Yes)
		{
			if(!QFile::remove(Main::dataPaths->demosDirectoryPath() + QDir::separator() + selectedDemo->filename))
				QMessageBox::critical(this, tr("Unable to delete"), tr("Could not delete the selected demo."));
			else
			{
				selectedDemo = NULL;

				// Adjust the tree
				QModelIndex index = demoList->selectionModel()->currentIndex();
				int dateRow = index.parent().row();
				int timeRow = index.row();

				demoModel->removeRow(timeRow, index.parent());
				demoTree[dateRow].removeAt(timeRow);
				if(demoTree[dateRow].size() == 0)
				{
					demoModel->removeRow(dateRow);
					demoTree.removeAt(dateRow);
				}
			}
		}
	}
	else // Play
	{
		if(selectedDemo == NULL)
			return;

		// Look for the plugin used to record.
		const EnginePlugin *plugin = NULL;
		for(int i = 0;i < Main::enginePlugins->numPlugins();i++)
		{
			if(selectedDemo->port == (*Main::enginePlugins)[i]->info->name)
				plugin = (*Main::enginePlugins)[i]->info->pInterface;
		}
		if(plugin == NULL)
		{
			QMessageBox::critical(this, tr("No plugin"), tr("The \"") + selectedDemo->port + tr("\" plugin does not appear to be loaded."));
			return;
		}

		// Locate all the files needed to play the demo
		PathFinder pf;
		PathFinderResult result = pf.findFiles(selectedDemo->wads);
		if(!result.missingFiles.isEmpty())
		{
			QMessageBox::critical(this, tr("Files not found"), tr("The following files could not be located: ") + result.missingFiles.join(", "));
			return;
		}

		// Play the demo
		Server *server = plugin->server(QHostAddress(), 5029); // No specific port needed since we're basically "playing offline"
		HostInfo hostInfo;
		hostInfo.demoPath = Main::dataPaths->demosDirectoryPath() + QDir::separator() + selectedDemo->filename;
		hostInfo.iwadPath = result.foundFiles[0];
		hostInfo.pwadsPaths = result.foundFiles.mid(1);

		GameRunner* gameRunner = server->gameRunner();
		Message message = gameRunner->host(hostInfo, GameRunner::DEMO);

		if (message.isError())
		{
			QMessageBox::critical(this, tr("Doomseeker - error"), message.content);
		}

		delete gameRunner;
		delete server;
	}
}

void DemoManagerDlg::updatePreview(const QModelIndex &index)
{
	if(!index.isValid() || !index.parent().isValid())
	{
		preview->setText("");
		selectedDemo = NULL;
		return;
	}

	int dateRow = index.parent().row();
	int timeRow = index.row();
	selectedDemo = &demoTree[dateRow][timeRow];

	QString text = "<b>" + tr("Port") + ":</b><p style=\"margin: 0px 0px 0px 10px\">" + selectedDemo->port + "</p>" +
		"<b>" + tr("Wads") + ":</b><p style=\"margin: 0px 0px 0px 10px\">";
	foreach(const QString &wad, selectedDemo->wads)
	{
		text += wad + "<br />";
	}
	text += "</p>";
	preview->setText(text);
}