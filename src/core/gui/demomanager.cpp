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
#include "plugins/engineplugin.h"
#include "serverapi/binaries.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "serverapi/gamehost.h"
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
	QPushButton *playButton = buttonBox->addButton(tr("Play"), QDialogButtonBox::ActionRole);
	playButton->setIcon(QIcon(":/icons/media-playback-start.png"));

	demoModel = new QStandardItemModel();
	adjustDemoList();

	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( performAction(QAbstractButton *) ));
	connect(demoList->selectionModel(), SIGNAL( currentChanged(const QModelIndex &, const QModelIndex &) ), this, SLOT( updatePreview(const QModelIndex &) ));
}

void DemoManagerDlg::adjustDemoList()
{
	// Get valid extensions
	QStringList demoExtensions;
	for(int i = 0;i < Main::enginePlugins->numPlugins();++i)
	{
		QString ext = QString("*.%1").arg((*Main::enginePlugins)[i]->info->data()->demoExtension);

		if(!demoExtensions.contains(ext))
		{
			demoExtensions << ext;
		}
	}

	// In order to index the demos we'll convert the dates to integers by calculating the days until today.
	// Also we need to convert double underscores to a single underscore
	QDate today = QDate::currentDate();
	QTime referenceTime(23, 59, 59);
	QDir demosDirectory(Main::dataPaths->demosDirectoryPath());
	QStringList demos = demosDirectory.entryList(demoExtensions, QDir::Files);
	typedef QMap<int, Demo> DemoMap;
	QMap<int, DemoMap> demoMap;
	foreach(const QString &demoName, demos)
	{
		QStringList demoData;
		QString metaData = demoName.left(demoName.lastIndexOf("."));
		// We need to split manually to handle escaping.
		for(unsigned int i = 0;i < metaData.length();++i)
		{
			if(metaData[i] == '_')
			{
				// If our underscore is followed by another just continue on...
				if(i+1 < metaData.length() && metaData[i+1] == '_')
				{
					++i;
					continue;
				}

				// Split the meta data and then restart from the beginning.
				demoData << metaData.left(i).replace("__", "_");
				metaData = metaData.mid(i+1);
				i = 0;
			}
		}
		// Whatever is left is a part of our data.
		demoData << metaData.replace("__", "_");
		if(demoData.size() < 3) // Should have at least 3 elements port, date, time[, iwad[, pwads]]
			continue;

		QDate date = QDate::fromString(demoData[1], "dd.MM.yyyy");
		QTime time = QTime::fromString(demoData[2], "hh.mm.ss");
		Demo demo;
		demo.filename = demoName;
		demo.port = demoData[0];
		demo.time = QDateTime(date, time);
		if(demoData.size() >= 4)
			demo.wads = demoData.mid(3);
		else
		{
			// New format, read meta data from file!
			Ini metaData(Main::dataPaths->demosDirectoryPath() + QDir::separator() + demoName + ".ini");
			demo.wads << metaData.retrieveSetting("meta", "iwad");
			QString pwads = metaData.retrieveSetting("meta", "pwads");
			if(pwads.length() > 0)
				demo.wads << pwads.split(";");
		}

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

bool DemoManagerDlg::doRemoveDemo(const QString &file)
{
	if(!QFile::remove(file))
		QMessageBox::critical(this, tr("Unable to delete"), tr("Could not delete the selected demo."));
	else
	{
		// Remove ini file as well, but don't bother warning if it can't be deleted for whatever reason
		QFile::remove(file + ".ini");
		selectedDemo = NULL;
		return true;
	}
	return false;
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
			QModelIndex index = demoList->selectionModel()->currentIndex();
			if(selectedDemo == NULL)
			{
				int dateRow = index.row();
				for(int timeRow = 0;index.child(timeRow, 0).isValid();++timeRow)
				{
					if(doRemoveDemo(Main::dataPaths->demosDirectoryPath() + QDir::separator() + demoTree[dateRow][timeRow].filename))
					{
						demoModel->removeRow(timeRow, index);
						demoTree[dateRow].removeAt(timeRow);
						if(demoTree[dateRow].size() == 0)
						{
							demoModel->removeRow(dateRow);
							demoTree.removeAt(dateRow);
							break;
						}

						// We deleted the top row, so decrement our pointer
						--timeRow;
					}
				}
			}
			else
			{
				if(doRemoveDemo(Main::dataPaths->demosDirectoryPath() + QDir::separator() + selectedDemo->filename))
				{
					// Adjust the tree
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
	}
	else // Play
	{
		if(selectedDemo == NULL)
			return;

		// Look for the plugin used to record.
		const EnginePlugin *plugin = NULL;
		for(int i = 0;i < Main::enginePlugins->numPlugins();i++)
		{
			if(selectedDemo->port == (*Main::enginePlugins)[i]->info->data()->name)
				plugin = (*Main::enginePlugins)[i]->info;
		}
		if(plugin == NULL)
		{
			QMessageBox::critical(this, tr("No plugin"), tr("The \"%1\" plugin does not appear to be loaded.").arg(selectedDemo->port));
			return;
		}

		// Create dummy server and get binary for pathfinder
		Server *server = plugin->server(QHostAddress(), 5029); // No specific port needed since we're basically "playing offline"
		Message binMessage;
		Binaries *bin = server->binaries();
		QString binPath = bin->clientBinary(binMessage);
		delete bin;

		// Locate all the files needed to play the demo
		PathFinder pf;
		pf.addPrioritySearchDir(binPath);
		PathFinderResult result = pf.findFiles(selectedDemo->wads);
		if(!result.missingFiles.isEmpty())
		{
			QMessageBox::critical(this, tr("Files not found"), tr("The following files could not be located: ") + result.missingFiles.join(", "));
			delete server;
			return;
		}

		// Play the demo
		HostInfo hostInfo;
		hostInfo.demoPath = Main::dataPaths->demosDirectoryPath() + QDir::separator() + selectedDemo->filename;
		hostInfo.iwadPath = result.foundFiles[0];
		hostInfo.pwadsPaths = result.foundFiles.mid(1);

		GameHost* gameRunner = server->gameHost();
		Message message = gameRunner->host(hostInfo, GameHost::DEMO);

		if (message.isError())
		{
			QMessageBox::critical(this, tr("Doomseeker - error"), message.contents());
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
		"<b>" + tr("WADs") + ":</b><p style=\"margin: 0px 0px 0px 10px\">";
	foreach(const QString &wad, selectedDemo->wads)
	{
		text += wad + "<br />";
	}
	text += "</p>";
	preview->setText(text);
}
