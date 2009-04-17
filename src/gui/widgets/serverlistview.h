#ifndef __SERVERLISTVIEW_H_
#define __SERVERLISTVIEW_H_

#include <QTableView>
#include <QMouseEvent>

class ServerListView : public QTableView
{
	Q_OBJECT

	public:
		ServerListView(QWidget* parent = 0) : QTableView(parent)
		{
		}

	protected:
		virtual void mouseReleaseEvent(QMouseEvent* event);

	signals:
		void rightMouseClick(const QModelIndex&);
};

#endif
