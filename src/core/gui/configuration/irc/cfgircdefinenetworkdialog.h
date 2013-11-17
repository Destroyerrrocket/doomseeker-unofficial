//------------------------------------------------------------------------------
// cfgircdefinenetworkdialog.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __CFGIRCDEFINENETWORKDIALOG_H__
#define __CFGIRCDEFINENETWORKDIALOG_H__

#include "ui_cfgircdefinenetworkdialog.h"
#include "irc/entities/ircnetworkentity.h"

class CFGIRCDefineNetworkDialog : public QDialog, private Ui::CFGIRCDefineNetworkDialog
{
	Q_OBJECT

	public:
		CFGIRCDefineNetworkDialog(const IRCNetworkEntity& initValuesEntity, QWidget* parent = NULL);
		CFGIRCDefineNetworkDialog(QWidget* parent = NULL);

		IRCNetworkEntity getNetworkEntity() const;

	public slots:
		void accept();

	private:
		bool askToAcceptAnywayWhenCommandsBad(const QStringList& offenders);
		QStringList autojoinCommands() const;
		void construct();
		QStringList formatOffenders(const QStringList& offenders) const;
		void initFrom(const IRCNetworkEntity& networkEntity);
		/**
		 * @brief If validation fails, the offending lines are returned.
		 */
		QStringList validateAutojoinCommands() const;

	private slots:
		void buttonClicked(QAbstractButton* button);

};

#endif
