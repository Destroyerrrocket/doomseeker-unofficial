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

#include <cassert>

class Application::PrivData
{
	public:
		int argc;
		char **argv;
};

Application *Application::staticInstance = NULL;

Application::Application(int argc, char **argv)
: QApplication(argc, argv)
{
	d = new PrivData();
	d->argc = argc;
	d->argv = argv;
}


Application::~Application()
{
	delete d;
}

void Application::deinit()
{
	if (staticInstance != NULL)
	{
		delete staticInstance;
		staticInstance = NULL;
	}
}

void Application::init(int argc, char **argv)
{
	assert(staticInstance == NULL && "Cannot initialize Application twice!");
	staticInstance = new Application(argc, argv);
}

Application *Application::instance()
{
	return staticInstance;
}
