//------------------------------------------------------------------------------
// cfgircsounds.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cfgircsounds.h"
#include "ui_cfgircsounds.h"
#include "irc/configuration/ircconfig.h"

#include <QFileDialog>

class CFGIRCSounds::PrivData : public Ui::CFGIRCSounds
{
};

CFGIRCSounds::CFGIRCSounds(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	d = new PrivData;
	d->setupUi(this);

	this->connect(d->btnBrowseNicknameUsed, SIGNAL( clicked() ), SLOT( btnBrowseNicknameUsedClicked() ) );
	this->connect(d->btnBrowsePrivateMessage, SIGNAL( clicked() ), SLOT( btnBrowsePrivateMessageClicked() ) );
}

CFGIRCSounds::~CFGIRCSounds()
{
	delete d;
}

void CFGIRCSounds::btnBrowseNicknameUsedClicked()
{
	setPath(d->leNicknameUsed, getPathToWav());
}

void CFGIRCSounds::btnBrowsePrivateMessageClicked()
{
	setPath(d->lePrivateMessage, getPathToWav());
}

QString CFGIRCSounds::getPathToWav()
{
	return QFileDialog::getOpenFileName(this, tr("Pick Sound File"),
		QString(),
		tr("WAVE (*.wav)"));
}

void CFGIRCSounds::readSettings()
{
	d->cbNicknameUsed->setChecked(gIRCConfig.sounds.bUseNicknameUsedSound);
	d->cbPrivateMessage->setChecked(gIRCConfig.sounds.bUsePrivateMessageReceivedSound);

	d->leNicknameUsed->setText(gIRCConfig.sounds.nicknameUsedSound);
	d->lePrivateMessage->setText(gIRCConfig.sounds.privateMessageReceivedSound);
}

void CFGIRCSounds::saveSettings()
{
	gIRCConfig.sounds.bUseNicknameUsedSound = d->cbNicknameUsed->isChecked();
	gIRCConfig.sounds.bUsePrivateMessageReceivedSound = d->cbPrivateMessage->isChecked();

	gIRCConfig.sounds.nicknameUsedSound = d->leNicknameUsed->text();
	gIRCConfig.sounds.privateMessageReceivedSound = d->lePrivateMessage->text();
}

void CFGIRCSounds::setPath(QLineEdit* pLineEdit, const QString& path)
{
	QString trimmedPath = path.trimmed();
	if (!trimmedPath.isEmpty())
	{
		pLineEdit->setText(trimmedPath);
	}
}
