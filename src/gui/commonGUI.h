#ifndef __COMMON_GUI_H
#define __COMMON_GUI_H

#include <QAbstractItemView>
#include <QListView>
#include <QStringList>

class CommonGUI
{
	public:
		/**
		 *	@brief Reads items from a QListView that uses QStandardItemModel 
		 *	and puts texts of these items into a list of strings.
		 *	@param listview - QListView that uses QStandardItemModel.
		 */
		static QStringList		listViewStandardItemsToStringList(QListView* listview);
		
		/**
		 *	@brief Removes all selected items from a QAbstractItemView.
		 *	@param view - QAbstractItemView from which items will be removed.
		 */
		static void 			removeSelectedItemsFromStandardItemView(QAbstractItemView* view);
};

#endif
