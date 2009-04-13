#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "ui_configureDlg.h"

class ConfigureDlg : public QDialog, private Ui::ConfigureDlg
{
	Q_OBJECT

	public:
		ConfigureDlg(QWidget* parent = 0);
};

#endif
