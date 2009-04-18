#include "gui/cfgFilePaths.h"
#include <QFileDialog>
#include <QStandardItem>

FilePathsConfigBox::FilePathsConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	QAbstractItemModel* model = new QStandardItemModel();
	lstIwadAndPwadPaths->setModel(model);

	connect(btnAddWadPath, SIGNAL( clicked() ), this, SLOT( btnAddWadPath_Click()) );
	connect(btnRemoveWadPath, SIGNAL( clicked() ), this, SLOT( btnRemoveWadPath_Click()) );
}
///////////////////////////////////////////////////
ConfigurationBoxInfo* FilePathsConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new FilePathsConfigBox(cfg, parent);
	ec->boxName = tr("File paths");
	return ec;
}
////////////////////////////////////////////////////
void FilePathsConfigBox::readSettings()
{
	SettingsData* setting;

	setting = config->setting("WadPaths");
	QStringList strList = setting->string().split(";", QString::SkipEmptyParts);
	for (int i = 0; i < strList.count(); ++i)
	{
		addPath(strList[i]);
	}
}

void FilePathsConfigBox::saveSettings()
{
	QStringList strList;
	SettingsData* setting;

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());
	{
		for(int i = 0; i < model->rowCount(); ++i)
		{
			QStandardItem* item = model->item(i);
			strList << item->text();
		}
	}

	setting = config->setting("WadPaths");
	setting->setValue(strList.join(";"));
}
////////////////////////////////////////////////////
void FilePathsConfigBox::addPath(const QString& strPath)
{
	if (strPath.isEmpty())
	{
		return;
	}

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());

	// Check if this path exists already, if so - do nothing.
	for(int i = 0; i < model->rowCount(); ++i)
	{
		QStandardItem* item = model->item(i);
		QString dir = item->text();
		Qt::CaseSensitivity cs;

		#ifdef Q_WS_WIN
		cs = Qt::CaseInsensitive;
		#else
		cs = Qt::CaseSensitive;
		#endif

		if (dir.compare(strPath, cs) == 0)
		{
			return;
		}
	}
	model->appendRow(new QStandardItem(strPath));
}
////////////////////////////////////////////////////
// Slots
void FilePathsConfigBox::btnAddWadPath_Click()
{
	QString strDir = QFileDialog::getExistingDirectory(this, tr("Doomseeker - Add wad path"));
	addPath(strDir);
}

void FilePathsConfigBox::btnRemoveWadPath_Click()
{
	QItemSelectionModel* selModel = lstIwadAndPwadPaths->selectionModel();
	QModelIndexList indexList = selModel->selectedIndexes();
	selModel->clear();

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstIwadAndPwadPaths->model());
	QList<QStandardItem*> itemList;
	for (int i = 0; i < indexList.count(); ++i)
	{
		itemList << model->itemFromIndex(indexList[i]);
	}

	for (int i = 0; i < itemList.count(); ++i)
	{
		QModelIndex index = model->indexFromItem(itemList[i]);
		model->removeRow(index.row());
	}



}
