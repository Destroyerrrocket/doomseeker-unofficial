//------------------------------------------------------------------------------
// miscserversetuppanel.cpp
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
#include "miscserversetuppanel.h"

#include "ini/ini.h"
#include "plugins/engineplugin.h"
#include "serverapi/gamecreateparams.h"

MiscServerSetupPanel::MiscServerSetupPanel(QWidget *parent)
: QWidget(parent)
{
	setupUi(this);
	anythingAvailable = false;
}

void MiscServerSetupPanel::fillInParams(GameCreateParams &params)
{
	params.setEmail(leEmail->text());
	params.setMotd(pteMOTD->toPlainText());
	params.setConnectPassword(leConnectPassword->text());
	params.setIngamePassword(leJoinPassword->text());
	params.setRconPassword(leRConPassword->text());
	params.setUrl(leURL->text());
}

bool MiscServerSetupPanel::isAnythingAvailable() const
{
	return anythingAvailable;
}

void MiscServerSetupPanel::loadConfig(Ini &config)
{
	IniSection misc = config.section("Misc");
	leURL->setText(misc["URL"]);
	leEmail->setText(misc["eMail"]);
	leConnectPassword->setText(misc["connectPassword"]);
	leJoinPassword->setText(misc["joinPassword"]);
	leRConPassword->setText(misc["RConPassword"]);
	pteMOTD->document()->setPlainText(misc["MOTD"]);
}

void MiscServerSetupPanel::saveConfig(Ini &config)
{
	IniSection misc = config.section("Misc");
	misc["URL"] = leURL->text();
	misc["eMail"] = leEmail->text();
	misc["connectPassword"] = leConnectPassword->text();
	misc["joinPassword"] = leJoinPassword->text();
	misc["RConPassword"] = leRConPassword->text();
	misc["MOTD"] = pteMOTD->toPlainText();
}

void MiscServerSetupPanel::setupForEngine(const EnginePlugin *engine)
{
	anythingAvailable = false;
	bool visible = false;

	visible = engine->data()->allowsConnectPassword;
	labelConnectPassword->setVisible(visible);
	leConnectPassword->setVisible(visible);
	anythingAvailable = visible || anythingAvailable;

	visible = engine->data()->allowsEmail;
	labelEmail->setVisible(visible);
	leEmail->setVisible(visible);
	anythingAvailable = visible || anythingAvailable;

	visible = engine->data()->allowsJoinPassword;
	labelJoinPassword->setVisible(visible);
	leJoinPassword->setVisible(visible);
	anythingAvailable = visible || anythingAvailable;

	visible = engine->data()->allowsMOTD;
	labelMOTD->setVisible(visible);
	pteMOTD->setVisible(visible);
	anythingAvailable = visible || anythingAvailable;

	visible = engine->data()->allowsRConPassword;
	labelRConPassword->setVisible(visible);
	leRConPassword->setVisible(visible);
	anythingAvailable = visible || anythingAvailable;

	visible = engine->data()->allowsURL;
	labelURL->setVisible(visible);
	leURL->setVisible(visible);
	anythingAvailable = visible || anythingAvailable;
}
