#ifndef SERVERCONSOLE_H
#define SERVERCONSOLE_H

#include <QLineEdit>
#include <QList>

#include "ui_serverconsole.h"
#include "gui/widgets/memorylineedit.h"

class ServerConsole : public QWidget, private Ui::ServerConsole
{
	Q_OBJECT

	public:
		ServerConsole(QWidget *parent=NULL, Qt::WindowFlags f=0);

	public slots:
		void	appendMessage(const QString &message);

	signals:
		void	messageSent(const QString &message);

	protected slots:
		void	forwardMessage();

	private:
		MemoryLineEdit	*consoleInput;
};

#endif // SERVERCONSOLE_H
