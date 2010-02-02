#include "commonGUI.h"
#include <QStandardItemModel>

QStringList CommonGUI::listViewStandardItemsToStringList(QListView* listview)
{
	QStringList list;
	QStandardItemModel* model = static_cast<QStandardItemModel*>(listview->model());
	for (int i = 0; i < model->rowCount(); ++i)
	{
		list << model->item(i)->text();
	}

	return list;
}

void CommonGUI::removeSelectedItemsFromStandardItemView(QAbstractItemView* view)
{
	QItemSelectionModel* selModel = view->selectionModel();
	QModelIndexList indexList = selModel->selectedIndexes();
	selModel->clear();

	QStandardItemModel* model = static_cast<QStandardItemModel*>(view->model());
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
