//------------------------------------------------------------------------------
// odamexmain.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include <QHostInfo>

#include "global.h"
#include "masterclient.h"
#include "sdeapi/pluginloader.hpp"

#include "odamex/engineOdamexConfig.h"
#include "odamex/odamexmasterclient.h"

class PLUGIN_EXPORT OdamexEnginePlugin : public EnginePlugin
{
	public:
		ConfigurationBoxInfo *configuration(Config *cfg, QWidget *parent) const
		{
			return EngineOdamexConfigBox::createStructure(cfg, parent);
		}

		MasterClient	*masterClient() const
		{
			QHostInfo info = QHostInfo::fromName("master1.odamex.net");
			if(info.addresses().size() == 0)
				return NULL;
			return new OdamexMasterClient(info.addresses().first(), 15000);
		}
};

static OdamexEnginePlugin odamex_engine_plugin;
static const PluginInfo odamex_info = {"Odamex", "Odamex server query plugin.", "The Skulltag Team", {0,1,0,0}, MAKEID('E','N','G','N'), &odamex_engine_plugin};
extern "C" PLUGIN_EXPORT const PluginInfo *doomSeekerInit()
{
	return &odamex_info;
}
