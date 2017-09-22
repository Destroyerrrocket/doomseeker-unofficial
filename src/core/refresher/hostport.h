//------------------------------------------------------------------------------
// hostport.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2017 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id769c5e06_8b14_4e1e_b71c_3a1632b446c3
#define id769c5e06_8b14_4e1e_b71c_3a1632b446c3

#include <QHostAddress>

class HostPort
{
public:
	QHostAddress host;
	unsigned short port;

	HostPort()
	{
		this->port = 0;
	}

	HostPort(const QHostAddress &host, unsigned short port)
	{
		this->host = host;
		this->port = port;
	}

	bool operator<(const HostPort &other) const
	{
		return host != other.host ? host.toString() < other.host.toString() : port < other.port;
	}

	bool operator==(const HostPort &other) const
	{
		return host == other.host && port == other.port;
	}
};

uint qHash(const HostPort &hostPort);

#endif
