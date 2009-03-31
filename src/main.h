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
			printf("%d	%s	%s\n", server->ping(), server->gameMode().name().toAscii().data(), server->name().toAscii().data());
		}
};

#endif /* __MAIN_H__ */
