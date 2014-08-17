//------------------------------------------------------------------------------
// fontbutton.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "fontbutton.h"
#include <QFontDialog>

FontButton::FontButton(QWidget* parent)
: QPushButton(parent)
{
	connect(this, SIGNAL( clicked() ), SLOT( thisClicked() ) );

	this->updateAppearance();
}

void FontButton::setSelectedFont(const QFont& font)
{
	this->currentFont = font;
	this->updateAppearance();
}

void FontButton::thisClicked()
{
	bool bOk = false;
	QFont fontTmp = QFontDialog::getFont(&bOk, this->currentFont, this->parentWidget());

	if(bOk)
	{
		this->updateFont(fontTmp);
	}
}

void FontButton::updateAppearance()
{
	QString text = QString("%1, %2").arg(this->currentFont.family()).arg(this->currentFont.pointSize());
	QFont textFont = this->currentFont;

	this->setFont(textFont);
	this->setText(text);
}

void FontButton::updateFont(const QFont& newFont)
{
	QFont oldFont = this->currentFont;
	this->currentFont = newFont;
	updateAppearance();

	emit fontUpdated(oldFont, this->currentFont);
}

