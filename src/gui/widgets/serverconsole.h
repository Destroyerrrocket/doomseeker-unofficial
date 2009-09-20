#ifndef SERVERCONSOLE_H
#define SERVERCONSOLE_H

#include "ui_serverconsole.h"

class ServerConsole : public QWidget, private Ui::ServerConsole
{
	Q_OBJECT

	public:
		ServerConsole(QWidget *parent=NULL, Qt::WindowFlags f=0) : QWidget(parent, f)
		{
			setupUi(this);

			connect(consoleInput, SIGNAL(returnPressed()), this, SLOT(forwardMessage()));
		}

	public slots:
		void	appendMessage(const QString &message)
		{
			QString appendMessage = message;
			if(appendMessage.endsWith('\n')) // Remove the trailing new line since appendPlainText seems to add one automatically.
				appendMessage.chop(1);

			consoleOutput->appendPlainText(appendMessage);
		}

	signals:
		void	messageSent(const QString &message);

	protected slots:
		void	forwardMessage()
		{
			emit messageSent(consoleInput->text());
			consoleInput->setText("");
		}
};

#endif // SERVERCONSOLE_H
