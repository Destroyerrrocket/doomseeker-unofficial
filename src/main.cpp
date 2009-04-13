// This is just a test function right now.
#include <QApplication>
#include <QObject>
#include <QThreadPool>

#include "gui/mainwindow.h"

int main(int argc, char* argv[])
{
	QThreadPool::globalInstance()->setMaxThreadCount(50);

	QApplication app(argc, argv);

	MainWindow* mw = new MainWindow();
	mw->show();
	return app.exec();
}
