//------------------------------------------------------------------------------
// lookuphost.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "lookuphost.h"

#include <QHostInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include "log.h"

LookupHost *LookupHost::inst = NULL;

class LookupHost::PrivData
{
public:
	/**
	 * Limiting max lookups improves quit times when
	 * hundreds of lookups were requested at once.
	 */
	static const int MAX_LOOKUPS = 20;

	QAtomicInt workCounter;
	int runningLookups;
	LookupHostConsumerThread thread;
	QMutex lock;
	bool accepting;
};

LookupHost::LookupHost()
{
	d = new PrivData();
	d->accepting = true;
	d->runningLookups = 0;
}

LookupHost::~LookupHost()
{
	delete d;
}

void LookupHost::lookupHost(const QString &name, QObject *receiver, const char *receiverSlot)
{
	instance()->lookupHost_(name, receiver, receiverSlot);
}

void LookupHost::lookupHost_(const QString &name, QObject *receiver, const char *receiverSlot)
{
	QMutexLocker locker(&d->lock);
	if (!d->accepting)
	{
		return;
	}
	d->workCounter.ref();

	QTimer *timer = new QTimer();
	LookupHostWorker *worker = new LookupHostWorker(name);

	QObject::connect(worker, SIGNAL(hostFound(QHostInfo)), receiver, receiverSlot);
	worker->connect(timer, SIGNAL(timeout()), SLOT(work()));
	timer->connect(timer, SIGNAL(timeout()), SLOT(deleteLater()));

	timer->start(0);

	worker->moveToThread(&d->thread);
	timer->moveToThread(&d->thread);
}

void LookupHost::finalizeAndJoin()
{
	if (inst != NULL)
	{
		gLog << tr("Finalizing LookupHost thread");
		instance()->finalizeAndJoin_();
		gLog << tr("Finalized LookupHost thread");
	}
}

void LookupHost::finalizeAndJoin_()
{
	{
		QMutexLocker locker(&d->lock);
		d->accepting = false;
	}
	while (d->workCounter > 0)
	{
		d->thread.wait(1000);
	}
	d->thread.quit();
}

void LookupHost::derefWorkCounter()
{
	d->workCounter.deref();
}

LookupHost *LookupHost::instance()
{
	if (inst == NULL)
	{
		inst = new LookupHost();
		inst->d->thread.start();
	}
	return inst;
}
////////////////////////////////////////////////////////////////////////////////
LookupHostWorker::LookupHostWorker(const QString &hostName)
{
	this->hostName = hostName;
}

void LookupHostWorker::work()
{
	QMutexLocker locker(&LookupHost::instance()->d->lock);
	if (LookupHost::instance()->d->accepting)
	{
		if (LookupHost::instance()->d->runningLookups < LookupHost::PrivData::MAX_LOOKUPS)
		{
			++LookupHost::instance()->d->runningLookups;
			QHostInfo::lookupHost(hostName, this, SLOT(hostFoundReceived(QHostInfo)));
		}
		else
		{
			// Wait a while and try again.
			QTimer::singleShot(1000, this, SLOT(work()));
		}
	}
	else 
	{
		LookupHost::instance()->derefWorkCounter();
		deleteLater();
	}
}

void LookupHostWorker::hostFoundReceived(const QHostInfo &hostInfo)
{
	QMutexLocker locker(&LookupHost::instance()->d->lock);
	--LookupHost::instance()->d->runningLookups;
	LookupHost::instance()->derefWorkCounter();
	emit hostFound(hostInfo);
	deleteLater();
}
////////////////////////////////////////////////////////////////////////////////
void LookupHostConsumerThread::run()
{
	qDebug() << "LookupHostConsumerThread:" << QThread::currentThreadId();
	exec();
}
