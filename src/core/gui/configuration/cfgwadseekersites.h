//------------------------------------------------------------------------------
// cfgwadseekersites.h
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
#ifndef __WADSEEKERCONFIG_SITES_H_
#define __WADSEEKERCONFIG_SITES_H_

#include "gui/configuration/configurationbasebox.h"
#include "dptr.h"
#include <QIcon>

class QUrl;

class CFGWadseekerSites : public ConfigurationBaseBox
{
	Q_OBJECT

	public:
		CFGWadseekerSites(QWidget* parent = NULL);
		~CFGWadseekerSites();

		QIcon icon() const { return QIcon(":/icons/emblem-web.png"); }
		QString name() const { return tr("Sites"); }
		void readSettings();
		QString title() const { return tr("Wadseeker - Sites"); }

	protected slots:
		void btnUrlAddClicked();
		void btnUrlDefaultClicked();
		void btnUrlRemoveClicked();

	protected:
		void saveSettings();

	private:
		DPtr<CFGWadseekerSites> d;

		void insertUrl(const QString& url);
};

#endif
