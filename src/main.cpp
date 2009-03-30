// This is just a test function right now.
#include <QApplication>
#include <QObject>

#include "main.h"
#include "skulltag/skulltagmasterclient.h"

int main(int argc, char* argv[])
{
	Tester *test = new Tester();
	QApplication app(argc, argv);
	SkulltagMasterClient mc(QHostAddress("91.121.87.67"), 15300);
	mc.refresh();
	for(int i = 0;i < mc.numServers();i++)
	{
		//printf("%s:%d\n", mc[i]->getAddress().toString().toAscii().data(), mc[i]->getPort());
		QObject::connect(mc[i], SIGNAL(updated(const Server *)), test, SLOT(serverUpdated(const Server *)));
		mc[i]->refresh();
	}
}
