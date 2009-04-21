#ifndef __DOCKEDWIDGETS_H_
#define __DOCKEDWIDGETS_H_

#include <QWidget>

class DockedWidgetServerInfo : public QWidget
{
	Q_OBJECT

	public:
		DockedWidgetServerInfo(QWidget* parent = NULL) : QWidget(parent)
		{
		}

		QSize sizeHint() const
		{
			return QSize(300, 500);
		}
};

#endif
