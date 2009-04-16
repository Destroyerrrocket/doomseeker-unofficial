#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include "main.h"
#include "sdeapi/config.hpp"
#include "ui_mainwindow.h"

#include <QString>
#include <QStandardItem>

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	public:
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
		const Server* serverFromList(int rowNum) const;
        const Server* serverFromList(const QModelIndex&) const;
        const Server* serverFromList(const QStandardItem*) const;
};

#endif
