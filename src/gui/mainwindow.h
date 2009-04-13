#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include "main.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	private:
		Tester* tester;

	public:
		MainWindow();
		~MainWindow();

	public slots:
		void refresh();
		void menuOptionsConfigure();

};

#endif
