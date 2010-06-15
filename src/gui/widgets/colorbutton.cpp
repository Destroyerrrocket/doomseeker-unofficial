//------------------------------------------------------------------------------
// colorbutton.cpp
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
#include "colorbutton.h"
#include <QColorDialog>

ColorButton::ColorButton(QWidget* parent)
: QPushButton(parent)
{
	connect( this, SIGNAL( clicked() ), this, SLOT( thisClicked() ) );
	
	color.setNamedColor("#ffffff");
	updateAppearance();
}

QString ColorButton::colorHtml() const
{
	return color.name();
}

unsigned ColorButton::colorUnsigned() const
{
	return color.rgb();
}

void ColorButton::setColor(unsigned colorValue)
{
	color.setRgb(colorValue);
	updateAppearance();
}

void ColorButton::thisClicked()
{
	QColor colorTmp = QColorDialog::getColor(QColor(color), parentWidget());

	if(colorTmp.isValid())
	{
		color = colorTmp.rgb();
		updateAppearance();
	}
}

void ColorButton::updateAppearance()
{
	static const QString COLOR_STYLE = "QPushButton { background-color : %1; }";
	
	QString styleSheet = COLOR_STYLE.arg(color.name());
	setStyleSheet(styleSheet);
}
