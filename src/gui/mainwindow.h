#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include "main.h"
#include "sdeapi/config.hpp"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	private:
		Config* config;
		Tester* tester;

	public:
		MainWindow(int argc, char** argv);
		~MainWindow();

	public slots:
		void refresh();
		void menuOptionsConfigure();

};

#endif
