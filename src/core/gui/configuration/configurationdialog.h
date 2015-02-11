//------------------------------------------------------------------------------
// configurationdialog.h
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __CONFIGUREDIALOG_H_
#define __CONFIGUREDIALOG_H_

#include "dptr.h"

#include <QDialog>

class ConfigurationBaseBox;
class QAbstractButton;
class QModelIndex;
class QStandardItem;
class QTreeView;

class ConfigurationDialog : public QDialog
{
	Q_OBJECT

	public:
		ConfigurationDialog(QWidget* parent = NULL);
		~ConfigurationDialog();

		/**
		 *	@brief Adds a new configuration box to the options tree view.
		 *
		 *	@param rootItem
		 *		Pointer to the root node of the tree to which the new box
		 *		will be attached. If NULL the standard model root is used.
		 *	@param pConfigurationBox
		 *		Pointer to configuration Group Box.
		 *	@param position
		 *		Passing <0 will use appendRow method, otherwise the new
		 *		tree view node will be inserted at specified position.
		 *
		 *	@return NULL if ConfigurationBox was not added. Pointer to a new
		 *	tree node if operation was successful.
		 */
		virtual QStandardItem* addConfigurationBox(QStandardItem* rootItem, ConfigurationBaseBox* pConfigurationBox, int position = -1);

		/**
		 *	@brief Adds a label node to the options tree view.
		 *
		 *	Such node has no ConfigurationBoxInfo attached. It serves only
		 *	organizational purposes.
		 *	@param rootItem
		 *		Pointer to the root node of the tree to which the new box
		 *		will be attached. If NULL the standard model root is used.
		 *	@param label
		 *		Label for the new node.
		  *	@param position
		 *		Passing <0 will use appendRow method, otherwise the new
		 *		tree view node will be inserted at specified position.
		 *
		 *	@return Newly created options tree view node. NULL if rootItem
		 *	was not a member of the tree view.
		 */
		QStandardItem* addLabel(QStandardItem* rootItem, const QString& label, int position = -1);
		bool isConfigurationBoxOnTheList(ConfigurationBaseBox* pConfigurationBox);


	protected:
		bool canConfigurationBoxBeAddedToList(ConfigurationBaseBox* pConfigurationBox);

		virtual void doSaveSettings() {};
		virtual void keyPressEvent(QKeyEvent* e);

		bool isConfigurationBoxInfoValid(ConfigurationBaseBox* pConfigurationBox);

		/**
		 * @brief Returns pointer to the tree widget that contains
		 *        configuration sections list.
		 */
		QTreeView* optionsTree();

		/**
		 * 	@param widget - hide currently displayed box if NULL.
		 */
		void showConfigurationBox(ConfigurationBaseBox* widget);
		void saveSettings();
		void switchToItem(const QModelIndex&);

		virtual bool validate() { return true; }

	protected slots:
		void btnClicked(QAbstractButton *button);
		void onOptionListCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

	private:
		DPtr<ConfigurationDialog> d;

		bool hasItemOnList(QStandardItem* pItem) const;
};

#endif
