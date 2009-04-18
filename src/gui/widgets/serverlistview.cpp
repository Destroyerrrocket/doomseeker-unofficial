#include "gui/widgets/serverlistview.h"
#include <QDebug>

void ServerListView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::RightButton)
	{
		QTableView::mouseReleaseEvent(event);
	}
	else
	{
		QModelIndex index = indexAt(event->pos());
		emit rightMouseClick(index);
	}
}

void ServerListView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		QTableView::mouseDoubleClickEvent(event);
	}
	else
	{
		QModelIndex index = indexAt(event->pos());
		emit leftMouseDoubleClicked(index);
	}
}
