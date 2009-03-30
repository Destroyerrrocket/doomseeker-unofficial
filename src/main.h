// Currently also just a test

#ifndef __MAIN_H__
#define __MAIN_H__

#include <QObject>

#include "server.h"

class Tester : public QObject
{
	Q_OBJECT

	public slots:
		void serverUpdated(const Server *server)
		{
			printf("%d	%s\n", server->getPing(), server->getName().toAscii().data());
		}
};

#endif /* __MAIN_H__ */
