#include "ircctcpparser.h"

#include <QDebug>
#include <QStringList>

class IRCCtcpParser::PrivData
{
	public:
		QString command;
		IRCCtcpParser::CtcpEcho echo;
		QString msg;
		QStringList params;
		QString printable;
		QString recipient;
		QString sender;
};

IRCCtcpParser::IRCCtcpParser(const QString &sender, const QString &recipient, const QString &msg)
{
	d = new PrivData();
	d->echo = DontShow;
	d->msg = msg;
	d->recipient = recipient;
	d->sender = sender;
}

IRCCtcpParser::~IRCCtcpParser()
{
	delete d;
}

IRCCtcpParser::CtcpEcho IRCCtcpParser::echo() const
{
	return d->echo;
}

bool IRCCtcpParser::isCommand(const QString &candidate)
{
	return d->command.compare(candidate, Qt::CaseInsensitive) == 0;
}

bool IRCCtcpParser::isCtcp() const
{
	if (d->msg.length() <= 2)
	{
		return false;
	}
	return d->msg[0] == 0x1 && d->msg[d->msg.length() - 1] == 0x1;
}

bool IRCCtcpParser::parse()
{
	if (!isCtcp())
	{
		return false;
	}
	tokenizeMsg();
	d->printable = tr("CTCP: [%1] %2 %3").arg(d->sender, d->command, d->params.join(" "));
	if (isCommand("action"))
	{
		d->echo = PrintAsNormalMessage;
		d->printable = tr("%1 %2").arg(d->sender, d->params.join(" "));
	}
	else
	{
		d->echo = DisplayInServerTab;
	}
	return true;
}

QString IRCCtcpParser::printable() const
{
	return d->printable;
}

void IRCCtcpParser::tokenizeMsg()
{
	QString stripped = d->msg.mid(1, d->msg.length() - 2);
	QStringList tokens = stripped.split(" ");
	d->command = tokens.takeFirst();
	d->params = tokens;
}
