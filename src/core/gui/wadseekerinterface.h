//------------------------------------------------------------------------------
// wadseekerinterface.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WADSEEKERINTERFACE_H_
#define __WADSEEKERINTERFACE_H_

#include "ui_wadseekerinterface.h"
#include "ini.h"
#include "wadseeker/wadseeker.h"
#include <QStringList>
#include <QTimer>

/**
 * This dialog box returns 'Accepted' result ONLY when
 * automaticCloseOnSuccess is set to true and when
 * it succeedes finding all wads.
 */
class WadSeekerInterface : public QDialog, Ui::WadSeekerInterface
{
	Q_OBJECT

	public:
		WadSeekerInterface(QWidget* parent = NULL);
		~WadSeekerInterface();

		bool 		isAutomatic() { return bAutomatic; }

		/**
		 * Sets the window to start seeking immediatelly after being shown and
		 * automatically close on success.
		 * @param b - is automatic or is it not
		 * @param seekedWads - wads to seek if 'b' parameter is set to true.
		 */
		void 		setAutomatic(bool b, const QStringList& seekedWads)
		{
			bAutomatic = b;
			this->seekedWads = seekedWads;
		}

		Wadseeker&	wadseekerRef() { return wadseeker; }

	protected:
		enum States
		{
			Downloading 	= 0,
			Waiting			= 1
		};

		static const int	UPDATE_INTERVAL_MS;

		bool			bAutomatic;
		bool			bFirstShown;
		bool			bNeedsUpdate;

		// Setup for customization in the future.
		QString			colorHtmlMessageFatalError;
		QString			colorHtmlMessageError;
		QString			colorHtmlMessageNotice;

		/**
		 * Interface uses this instead of line edit if bAutomatic is true.
		 */
		QStringList 	seekedWads;
		States			state;
		QTimer			updateTimer;
		Wadseeker		wadseeker;

		void			displayMessage(const QString& message, Wadseeker::MessageType type, bool bPrependErrorsWithMessageType);
		void			fail();
		void			initMessageColors();

		/**
		 * @brief Sets default window title to default.
		 */
		void            resetTitleToDefault();

		void			setStateDownloading();
		void			setStateWaiting();
		void			setupIdgames();
		void 			showEvent(QShowEvent* event);
		void			startSeeking(const QStringList& seekedFilesList);
		void            updateTitle();

	protected slots:
		void	accept();
		void	aborted();
		void 	allDone();
		void 	downloadProgress(int done, int total);
		void	reject();
		void	message(const QString& message, Wadseeker::MessageType type);
		void	registerUpdateRequest();
};

#endif
