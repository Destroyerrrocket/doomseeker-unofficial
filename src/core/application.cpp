//------------------------------------------------------------------------------
// application.cpp
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
#include "application.h"

#include "gui/mainwindow.h"
#include <cassert>

class Application::PrivData
{
	public:
		QApplication *application;
		MainWindow *mainWindow;
		bool running;
		QString workingDirectory;
};

Application *Application::staticInstance = NULL;

Application::Application(QApplication *application)
{
	d = new PrivData();
	d->application = application;
	d->mainWindow = NULL;
	d->running = true;
	d->workingDirectory = "./";
}


Application::~Application()
{
	delete d;
}

QApplication *Application::applicationQt()
{
	return d->application;
}

void Application::deinit()
{
	if (staticInstance != NULL)
	{
		staticInstance->destroy();
	}
}

void Application::destroy()
{
	d->running = false;
}

int Application::exec()
{
	return d->application->exec();
}

void Application::init(QApplication *application)
{
	assert(staticInstance == NULL && "Cannot initialize Application twice!");
	staticInstance = new Application(application);
}

Application *Application::instance()
{
	assert(staticInstance != NULL);
	return staticInstance;
}

bool Application::isRunning() const
{
	return d->running;
}

MainWindow *Application::mainWindow() const
{
	return d->mainWindow;
}

QWidget *Application::mainWindowAsQWidget() const
{
	return d->mainWindow;
}

void Application::setMainWindow(MainWindow *mainWindow)
{
	d->mainWindow = mainWindow;
}

void Application::stopRunning()
{
	d->running = false;
}
