//------------------------------------------------------------------------------
// flagspagevaluecontroller2.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_PLUGIN_ZANDRONUM_CREATESERVERDIALOGPAGES_FLAGSPAGEVALUECONTROLLER2_H
#define DOOMSEEKER_PLUGIN_ZANDRONUM_CREATESERVERDIALOGPAGES_FLAGSPAGEVALUECONTROLLER2_H

#include <QAbstractButton>
#include <QMap>

#include "createserverdialogpages/flagspagevaluecontroller.h"

class FlagsPage;

namespace Zandronum2
{

/**
 * @brief Converts numerical flags values to widget representation
 *        and vice-versa.
 */
class FlagsPageValueController : public ::FlagsPageValueController
{
	public:
		FlagsPageValueController(FlagsPage* flagsPage);

		void convertNumericalToWidgets();
		void convertWidgetsToNumerical();

	private:
		unsigned compatflags;
		unsigned zandronumCompatflags;

		unsigned dmflags;
		unsigned dmflags2;
		unsigned zandronumDmflags;

		unsigned lmsAllowedWeapons;
		unsigned lmsSpectatorSettings;

		QMap<unsigned, QAbstractButton*> compatflagsCheckboxes;
		QMap<unsigned, QAbstractButton*> dmflagsCheckboxes;
		QMap<unsigned, QAbstractButton*> dmflags2Checkboxes;
		QMap<unsigned, QAbstractButton*> lmsAllowedWeaponsCheckboxes;
		QMap<unsigned, QAbstractButton*> lmsSpectatorSettingsCheckboxes;
		QMap<unsigned, QAbstractButton*> zandronumCompatflagsCheckboxes;
		QMap<unsigned, QAbstractButton*> zandronumDmflagsCheckboxes;

		FlagsPage* flagsPage;

		void convertToNumericalGeneral();
		void convertToNumericalPlayers();
		void convertToNumericalCooperative();
		void convertToNumericalDeathmatch();

		void convertCheckboxesToNumerical(const QMap<unsigned, QAbstractButton*>& checkboxMap,
			unsigned& flagsValue);
		void convertNumericalToCheckboxes(QMap<unsigned, QAbstractButton*>& checkboxMap,
			unsigned flagsValue);

		void convertToWidgetGeneral();
		void convertToWidgetPlayers();
		void convertToWidgetCooperative();
		void convertToWidgetDeathmatch();

		void readFlagsFromTextInputs();
};

}

#endif
