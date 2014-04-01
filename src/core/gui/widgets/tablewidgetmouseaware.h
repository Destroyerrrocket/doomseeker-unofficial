//------------------------------------------------------------------------------
// tablewidgetmouseaware.h
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __TABLEWIDGETMOUSEAWARE_H__
#define __TABLEWIDGETMOUSEAWARE_H__

#include <QList>
#include <QTableWidget>

class TableWidgetMouseAware : public QTableWidget
{
	Q_OBJECT
	
	public:
		TableWidgetMouseAware(QWidget* parent = NULL);
		
		bool isEmitSignalsEvenIfIndexIsInvalidEnabled() const { return bEmitSignalsEvenIfIndexIsInvalid; }
		
		/**
		 *	@brief Returns indices of currently selected rows.
		 */
		QList<int> selectedRows() const;
		void setEmitSignalsEvenIfIndexIsInvalid(bool b) { bEmitSignalsEvenIfIndexIsInvalid = b; }
		
	private:
		/**
		 *	@brief If set to true signals will be emitted without prior check
		 *	to see if a table item was clicked.
		 */
		bool bEmitSignalsEvenIfIndexIsInvalid;
	
	protected:
		virtual void mouseDoubleClickEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);

	signals:
		void leftMouseDoubleClicked(const QModelIndex& index, const QPoint& cursorPosition);
		void middleMouseClick(const QModelIndex& index, const QPoint& cursorPosition);
		void rightMouseClick(const QModelIndex& index, const QPoint& cursorPosition);
};

#endif
