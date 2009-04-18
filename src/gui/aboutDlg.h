#ifndef __ABOUTDIALOG_H__
#define __ABOUTDIALOG_H__

#include "ui_aboutDlg.h"

class AboutDlg : public QDialog, private Ui::aboutDlg
{
	public:
		AboutDlg(QWidget* parent = NULL);
		~AboutDlg();
};

#endif /* __ABOUTDIALOG_H__ */
