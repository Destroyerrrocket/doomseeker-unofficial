#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include "main.h"
#include "sdeapi/config.hpp"
#include "ui_mainwindow.h"

#include <QString>

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	public:
		enum ServerListColumns
		{
			SLC_PLAYERS = 0,
			SLC_PING = 1,
			SLC_SERVERNAME = 2,
			SLC_ADDRESS = 3,
			SLC_IWAD = 4,
			SLC_MAP = 5,
			SLC_WADS = 6,
			SLC_GAMETYPE = 7,
		};

		enum ServerListDataTypes
		{
			SLDT_POINTER_TO_SERVER_STRUCTURE = Qt::UserRole+1
		};

		MainWindow(int argc, char** argv);
		~MainWindow();

	public slots:
		void serverUpdated(const Server *server);
		void refresh();
		void menuOptionsConfigure();

	private:
		//QMutex		serverTableMutex;
		Config* 			config;
		Tester* 			tester;

		void prepareServerTable();
		QModelIndex findServerOnTheList(const Server* server);
		void addServer(const Server* server);
		void updateServer(const QModelIndex&, const Server* server);
};

#endif
