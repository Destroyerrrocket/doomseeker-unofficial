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
#include "ui_cfgchatlogspage.h"

#include "irc/configuration/chatlogscfg.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QUrl>

DClass<CfgChatLogsPage> : public Ui::CfgChatLogsPage
{
};

DPointered(CfgChatLogsPage)


CfgChatLogsPage::CfgChatLogsPage(QWidget *parent)
: ConfigurationBaseBox(parent)
{
	d->setupUi(this);
}

CfgChatLogsPage::~CfgChatLogsPage()
{
}

void CfgChatLogsPage::browseStorageDirectory()
{
	QString path = QFileDialog::getExistingDirectory(this,
		tr("Browse chat logs storage directory"), d->leDir->text());
	if (!path.isEmpty())
	{
		d->leDir->setText(path);
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
	if (checkDir(d->leDir->text()))
	{
		QString path = QDir::toNativeSeparators(d->leDir->text());
		QDesktopServices::openUrl(QString("file:///%1").arg(path));
	}
}

void CfgChatLogsPage::readSettings()
{
	ChatLogsCfg cfg;
	d->leDir->setText(cfg.chatLogsRootDir());
	d->cbStoreLogs->setChecked(cfg.isStoreLogs());
	d->cbRestoreLogs->setChecked(cfg.isRestoreChatFromLogs());
	d->groupRemoveOldArchives->setChecked(cfg.isRemoveOldLogs());
	d->spinLogRemovalAge->setValue(cfg.oldLogsRemovalDaysThreshold());
}

void CfgChatLogsPage::saveSettings()
{
	ChatLogsCfg cfg;
	cfg.setChatLogsRootDir(d->leDir->text());
	cfg.setStoreLogs(d->cbStoreLogs->isChecked());
	cfg.setRestoreChatFromLogs(d->cbRestoreLogs->isChecked());
	cfg.setRemoveOldLogs(d->groupRemoveOldArchives->isChecked());
	cfg.setOldLogsRemovalDaysThreshold(d->spinLogRemovalAge->value());
}
