//------------------------------------------------------------------------------
// cfgircsounds.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cfgircsounds.h"
#include "irc/configuration/ircconfig.h"

#include <QFileDialog>

CFGIRCSounds::CFGIRCSounds(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);

	this->connect(btnBrowseNicknameUsed, SIGNAL( clicked() ), SLOT( btnBrowseNicknameUsedClicked() ) );
	this->connect(btnBrowsePrivateMessage, SIGNAL( clicked() ), SLOT( btnBrowsePrivateMessageClicked() ) );
}

CFGIRCSounds::~CFGIRCSounds()
{
}

void CFGIRCSounds::btnBrowseNicknameUsedClicked()
{
	setPath(leNicknameUsed, getPathToWav());
}

void CFGIRCSounds::btnBrowsePrivateMessageClicked()
{
	setPath(lePrivateMessage, getPathToWav());
}

QString CFGIRCSounds::getPathToWav()
{
	return QFileDialog::getOpenFileName(this, tr("Pick Sound File"),
		QString(),
		tr("WAVE (*.wav)"));
}

void CFGIRCSounds::readSettings()
{
	cbNicknameUsed->setChecked(gIRCConfig.sounds.bUseNicknameUsedSound);
	cbPrivateMessage->setChecked(gIRCConfig.sounds.bUsePrivateMessageReceivedSound);

	leNicknameUsed->setText(gIRCConfig.sounds.nicknameUsedSound);
	lePrivateMessage->setText(gIRCConfig.sounds.privateMessageReceivedSound);
}

void CFGIRCSounds::saveSettings()
{
	gIRCConfig.sounds.bUseNicknameUsedSound = cbNicknameUsed->isChecked();
	gIRCConfig.sounds.bUsePrivateMessageReceivedSound = cbPrivateMessage->isChecked();

	gIRCConfig.sounds.nicknameUsedSound = leNicknameUsed->text();
	gIRCConfig.sounds.privateMessageReceivedSound = lePrivateMessage->text();
}

void CFGIRCSounds::setPath(QLineEdit* pLineEdit, const QString& path)
{
	QString trimmedPath = path.trimmed();
	if (!trimmedPath.isEmpty())
	{
		pLineEdit->setText(trimmedPath);
	}
}
