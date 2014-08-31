//------------------------------------------------------------------------------
// cfgchatlogspage.cpp
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
#include "cfgchatlogspage.h"

#include "irc/configuration/chatlogscfg.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QUrl>

class CfgChatLogsPage::PrivData
{
public:
};


CfgChatLogsPage::CfgChatLogsPage(QWidget *parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);
	d = new PrivData();
}

CfgChatLogsPage::~CfgChatLogsPage()
{
	delete d;
}

void CfgChatLogsPage::browseStorageDirectory()
{
	QString path = QFileDialog::getExistingDirectory(this,
		tr("Browse chat logs storage directory"), leDir->text());
	if (!path.isEmpty())
	{
		leDir->setText(path);
	}
}

bool CfgChatLogsPage::checkDir(const QString &directory)
{
	if (directory.trimmed().isEmpty())
	{
		QMessageBox::critical(this, tr("Directory error"), tr("Directory not specified."));
		return false;
	}

	QFileInfo dir(directory);
	if (!dir.exists())
	{
		QMessageBox::critical(this, tr("Directory error"), tr("Directory doesn't exist."));
		return false;
	}
	if (!dir.isDir())
	{
		QMessageBox::critical(this, tr("Directory error"), tr("Specified path isn't a directory."));
		return false;
	}
	return true;
}

void CfgChatLogsPage::exploreStorageDirectory()
{
	if (checkDir(leDir->text()))
	{
		QString path = QDir::toNativeSeparators(leDir->text());
		QDesktopServices::openUrl(QString("file:///%1").arg(path));
	}
}

void CfgChatLogsPage::readSettings()
{
	ChatLogsCfg cfg;
	leDir->setText(cfg.chatLogsRootDir());
	cbStoreLogs->setChecked(cfg.isStoreLogs());
	cbRestoreLogs->setChecked(cfg.isRestoreChatFromLogs());
}

void CfgChatLogsPage::saveSettings()
{
	ChatLogsCfg cfg;
	cfg.setChatLogsRootDir(leDir->text());
	cfg.setStoreLogs(cbStoreLogs->isChecked());
	cfg.setRestoreChatFromLogs(cbRestoreLogs->isChecked());
}
