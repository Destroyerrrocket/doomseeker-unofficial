#include "aboutDlg.h"
#include "global.h"

AboutDlg::AboutDlg(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	versionNumber->setText(VERSION);

	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( close() ));
}

AboutDlg::~AboutDlg()
{
}
