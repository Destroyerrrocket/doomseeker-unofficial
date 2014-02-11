//------------------------------------------------------------------------------
// application.h
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
#ifndef id5E252D92_36B9_40C2_964DBFD7D0E99622
#define id5E252D92_36B9_40C2_964DBFD7D0E99622

#include "global.h"

#include <QApplication>

#define gApp (Application::instance())

class MainWindow;

/**
 * @brief Program central hub of information.
 */
class MAIN_EXPORT Application : public QApplication
{
	public:
		/**
		 * @brief Doesn't delete the actual instance() but calls destroy()
		 *        method.
		 *
		 * That way certain data fields can still be accessed after the
		 * program is deinitialized. Actual instance is never deleted
		 * explicitly, and OS will clear the memory anyway.
		 */
		static void deinit();
		static bool isInit();
		static void init(int &argc, char **argv);
		static Application *instance();

		virtual ~Application();

		/**
		 * @brief Plugins and other threads can use this to figure out
		 *        that program is closing.
		 *
		 * Threads should exit their loops and let the program close gracefully.
		 */
		bool isRunning() const;

		/**
		 * @brief MainWindow of the program.
		 *
		 * Might be NULL if current run doesn't create MainWindow.
		 */
		MainWindow *mainWindow() const;
		/**
		 * @brief Returns MainWindow as a QWidget.
		 *
		 * Useful for plugins that need to specify parent widget for dialog
		 * boxes or such.
		 */
		QWidget *mainWindowAsQWidget() const;

		void setMainWindow(MainWindow* mainWindow);
		/**
		 * @brief Called when program is shutting down.
		 */
		void stopRunning();

	private:
		class PrivData;
		PrivData *d;

		static Application *staticInstance;

		Application(int &argc, char **argv);
		void destroy();
};


#endif
