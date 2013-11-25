//------------------------------------------------------------------------------
// serverpassword.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "serverpassword.h"

class ServerSimilarity
{
	public:
		ServerPassword::Server server;
		float similarity;

		ServerSimilarity(const ServerPassword::Server& server, float similarity)
		{
			this->server = server;
			this->similarity = similarity;
		}

		bool operator<(const ServerSimilarity& other) const
		{
			return similarity < other.similarity;
		}
};

void ServerPassword::addServer(const Server& v)
{
	if (v.isValid())
	{
		removeServer(v.game(), v.address(), v.port());
		d.servers.append(v);
	}
}

ServerPassword ServerPassword::deserializeQVariant(const QVariant& v)
{
	QVariantMap m = v.toMap();
	ServerPassword o;
	o.setPhrase(m["phrase"].toString());
	QVariantList variantServers = m["servers"].toList();
	foreach (QVariant server, variantServers)
	{
		o.addServer(Server::deserializeQVariant(server));
	}
	return o;
}

QString ServerPassword::lastGame() const
{
	return lastServer().game();
}

ServerPassword::Server ServerPassword::lastServer() const
{
	Server lastServer;
	foreach (const Server& s, d.servers)
	{
		if (!lastServer.isValid())
		{
			lastServer = s;
			continue;
		}

		if (s.isValid() && s.time() > lastServer.time())
		{
			lastServer = s;
		}
	}
	return lastServer;
}

QString ServerPassword::lastServerName() const
{
	return lastServer().name();
}

QDateTime ServerPassword::lastTime() const
{
	return lastServer().time();
}

ServerPassword::Server ServerPassword::mostSimilarServer(const Server& other, float* outSimilarity) const
{
	QList<ServerSimilarity> similarities;
	foreach (const Server& candidate, d.servers)
	{
		if (candidate.similarity(other) > 0.0f)
		{
			similarities << ServerSimilarity(candidate, candidate.similarity(other));
		}
	}
	if (!similarities.empty())
	{
		qSort(similarities);
		if (outSimilarity != NULL)
		{
			*outSimilarity = similarities.last().similarity;
		}
		return similarities.last().server;
	}
	else
	{
		if (outSimilarity != NULL)
		{
			*outSimilarity = 0.0f;
		}
		return Server(); // Invalid value.
	}
}

void ServerPassword::removeServer(const QString& game, const QString& address, unsigned short port)
{
	QMutableListIterator<Server> it(d.servers);
	while (it.hasNext())
	{
		Server server = it.next();
		if (server.game() == game && server.address() == address && server.port() == port)
		{
			it.remove();
		}
	}
}

QVariant ServerPassword::serializeQVariant() const
{
	QVariantMap m;
	m.insert("phrase", d.phrase);
	QVariantList variantServers;
	foreach (const Server& s, d.servers)
	{
		variantServers << s.serializeQVariant();
	}
	m.insert("servers", variantServers);
	return m;
}
////////////////////////////////////////////////////////////////////////////////
ServerPassword::Server ServerPassword::Server::deserializeQVariant(const QVariant& v)
{
	QVariantMap m = v.toMap();
	Server o;
	o.setAddress(m["address"].toString());
	o.setGame(m["game"].toString());
	o.setName(m["name"].toString());
	o.setPort(m["port"].toUInt());
	o.setTime(m["time"].toDateTime());
	return o;
}

QVariant ServerPassword::Server::serializeQVariant() const
{
	QVariantMap m;
	m.insert("address", d.address);
	m.insert("game", d.game);
	m.insert("name", d.name);
	m.insert("port", d.port);
	m.insert("time", d.time);
	return m;
}

float ServerPassword::Server::similarity(const Server& other) const
{
	if (!isValid() || !other.isValid())
	{
		return 0.0f;
	}

	const float MAX_SIMILARITY = 3.0f;
	float similarity = 0.0f;
	if (address() == other.address() && port() == other.port() && game() == other.game())
	{
		similarity += 2.0f;
	}
	if (name() == other.name())
	{
		similarity += 1.0f;
	}
	return similarity / MAX_SIMILARITY;
}
