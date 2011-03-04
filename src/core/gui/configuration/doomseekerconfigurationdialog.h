//------------------------------------------------------------------------------
// doomseekerconfigurationdialog.h
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
#ifndef __DOOMSEEKERCONFIGURATIONDIALOG_H__
#define __DOOMSEEKERCONFIGURATIONDIALOG_H__

#include "gui/configuration/configurationdialog.h"
		
class DoomseekerConfigurationDialog : public ConfigurationDialog
{
	Q_OBJECT

	public:
		DoomseekerConfigurationDialog(QWidget* parent = NULL);
		
		QStandardItem*					addConfigurationBox(QStandardItem* rootItem, ConfigurationBaseBox* pConfigurationBox, int position = -1);	
		bool 							addEngineConfiguration(ConfigurationBaseBox* pConfigurationBox);
		bool							appearanceChanged() const { return bAppearanceChanged; }
		bool 							customServersChanged() const { return bCustomServersChanged; }
		void 							initOptionsList();	
	
	private:
		bool							bAppearanceChanged;
		bool							bCustomServersChanged;
		ConfigurationBaseBox*			customServersCfgBox;		
		QStandardItem*					enginesRoot;			
	
		void							appendWadseekerConfigurationBoxes();
		void							doSaveSettings();
		
	private slots:
		/**
		 *	When called, sets bAppearanceChanged to true.
		 */
		void							appearanceChangedSlot();		
};

#endif
