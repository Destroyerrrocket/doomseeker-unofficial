#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include "main.h"
#include "sdeapi/config.hpp"
#include "gui/serverlist.h"
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
		void checkRefreshFinished();
		void btnRefresh_Click();
		void menuOptionsConfigure();

	private:
		SLHandler*			serverTableHandler;
		Config* 			config;
		Tester* 			tester;
};

#endif
