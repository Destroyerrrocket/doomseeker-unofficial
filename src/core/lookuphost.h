//------------------------------------------------------------------------------
// lookuphost.h
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id98fdc6b8_d51c_4a3b_a61e_91ddc4105daf
#define id98fdc6b8_d51c_4a3b_a61e_91ddc4105daf

#include "dptr.h"

#include <QObject>
#include <QString>
#include <QThread>

class QHostInfo;

/**
 * QHostInfo (Qt 4.8.6) has a bug on Windows that prevents the
 * program from exitting properly. If you return from main()
 * while a lookupHost() is in progress, the program will hang in limbo
 * forever. Its process will be visible in the task manager. Its
 * executable will remain locked for writing.
 *
 * Aborting the lookup before returning from main() doesn't solve the problem.
 *
 * Long lookupHost() can be easily caused by looking up a LAN address
 * such as 10.0.0.2 or 192.168.0.20, both if that address exists in your
 * network or doesn't even make any sense (ie. your network is class C and you
 * try to get address from class A). Another case is where you try to
 * resolve an actual domain, such as www.example.org, and your Internet
 * connection is down.
 *
 * This problem can be worked around by looking up hosts in a separate thread
 * and keeping count on how many lookups are running at the moment.
 * The main thread can synchronize with lookup thread and wait for all lookups
 * to finish before returning from main().
 *
 * Naturally, on non-Windows OSes all works fine...
 *
 * What's even more interesting, is that QNetworkAccessManager doesn't seem
 * to be causing any trouble when you try to resolve a domain name
 * while your Internet connection is down.
 */
class LookupHost : public QObject
{
Q_OBJECT

friend class LookupHostAborter;
friend class LookupHostWorker;

public:
	static void finalizeAndJoin();
	static void lookupHost(const QString &name, QObject *receiver, const char *receiverSlot);

private:
	DPtr<LookupHost> d;

	static LookupHost *instance();
	static LookupHost *inst;

	LookupHost();
	virtual ~LookupHost();

	void finalizeAndJoin_();
	void lookupHost_(const QString &name, QObject *receiver, const char *receiverSlot);

private slots:
	void derefWorkCounter();
};

class LookupHostWorker : public QObject
{
Q_OBJECT

public:
	LookupHostWorker(const QString &hostName);

signals:
	void hostFound(const QHostInfo &);

public slots:
	void work();

private:
	QString hostName;

private slots:
	void hostFoundReceived(const QHostInfo&);
};


class LookupHostConsumerThread : public QThread
{
Q_OBJECT

public:
	void run();
};

#endif
