#ifndef SERVERCONSOLE_H
#define SERVERCONSOLE_H

#include <QLineEdit>
#include <QList>

#include "ui_serverconsole.h"

/**
 * Stores the input into a history that can be accessed by pressing up and down.
 */
class MemoryLineEdit : public QLineEdit
{
	Q_OBJECT

	public:
		MemoryLineEdit(QWidget *parent=0);

	protected:
		void	keyPressEvent(QKeyEvent *event);

	protected slots:
		void	storeCommand();

	private:
		QList<QString>				history;
		QList<QString>::iterator	position;
};

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
