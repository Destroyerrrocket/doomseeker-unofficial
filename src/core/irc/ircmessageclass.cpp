//------------------------------------------------------------------------------
// ircmessageclass.cpp
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
#include "ircmessageclass.h"
#include "irc/configuration/ircconfig.h"

QString IRCMessageClass::colorFromConfig() const
{
	IRCConfig::AppearanceCfg& appearance = gIRCConfig.appearance;
	switch (className)
	{
		case ChannelAction:
			return appearance.channelActionColor;
			
		case Error:
			return appearance.errorColor;
			
		case Normal:
			return appearance.defaultTextColor;
			
		case NetworkAction:
			return appearance.networkActionColor;
		
		default:
			return "#000000";
	}
}

QString IRCMessageClass::toStyleSheetClassName(ClassName className)
{
	switch (className)
	{
		case ChannelAction:
			return "channelAction";
			
		case Error:
			return "error";
			
		case Normal:
			return "";
			
		case NetworkAction:
			return "networkAction";
		
		default:
			return "appErr";
	}
}
