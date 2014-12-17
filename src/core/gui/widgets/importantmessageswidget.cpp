//------------------------------------------------------------------------------
// importantmessageswidget.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "importantmessageswidget.h"

#include <QDateTime>
#include "ui_importantmessageswidget.h"

class ImportantMessagesWidget::PrivData : public Ui::ImportantMessagesWidget
{
public:
	class MessageLabel
	{
	public:
		MessageLabel(QLabel* pLabel)
		{
			this->pLabel = pLabel;
			this->timeCreated = QDateTime::currentDateTime();
		}

		QLabel* pLabel;
		QDateTime timeCreated;
	};

	static const unsigned DEFAULT_MAX_MESSAGES = 0;
	static const unsigned MAX_MSG_KEEP_TIME_SEC = 10;

	QList<MessageLabel> labelWidgets;
	int maxMessages;
};

ImportantMessagesWidget::ImportantMessagesWidget(QWidget* pParent)
: QWidget(pParent)
{
	d = new PrivData();
	d->maxMessages = PrivData::DEFAULT_MAX_MESSAGES;
	d->setupUi(this);

	this->hide();
}

ImportantMessagesWidget::~ImportantMessagesWidget()
{
	delete d;
}

void ImportantMessagesWidget::addMessage(const QString& message)
{
	addMessage(message, QDateTime());
}

void ImportantMessagesWidget::addMessage(const QString& message, const QDateTime& dateTime)
{
	QString strTimestamp = "";
	if (dateTime.isValid())
	{
		strTimestamp = dateTime.toString("[hh:mm:ss]") + " ";
	}

	QString formattedMessage = "<p>" + strTimestamp + message + "</p>";

	QFont font;
	font.setPointSize(qMax(1, font.pointSize() - 1));

	QLabel* pNewLabel = new QLabel(this);
	pNewLabel->setFont(font);
	pNewLabel->setText(formattedMessage);
	pNewLabel->setTextInteractionFlags(pNewLabel->textInteractionFlags()
		| Qt::TextSelectableByMouse);
	pNewLabel->setWordWrap(true);

	d->labelWidgets << PrivData::MessageLabel(pNewLabel);
	d->messageLayout->addWidget(pNewLabel);

	// Remember that widget may be auto-hidden.
	this->show();

	// Do not forget co clear oldest widgets if necessary.
	dropOldWidgetsIfBeyondLimit();
}

void ImportantMessagesWidget::addMessage(const QString& message, unsigned timestamp)
{
	addMessage(message, QDateTime::fromTime_t(timestamp));
}

void ImportantMessagesWidget::clear()
{
	while (!d->labelWidgets.isEmpty())
	{
		removeOneOldest();
	}
}

void ImportantMessagesWidget::dropOldWidgetsIfBeyondLimit()
{
	while (d->labelWidgets.size() > d->maxMessages)
	{
		PrivData::MessageLabel& oldestLabel = d->labelWidgets.first();
		int timeDifference = oldestLabel.timeCreated.secsTo(QDateTime::currentDateTime());

		if (timeDifference > PrivData::MAX_MSG_KEEP_TIME_SEC)
		{
			removeOneOldest();
		}
		else
		{
			// Exit the loop as no message is old enough to be dropped.
			// Messages are ordered in chronological order.
			break;
		}
	}
}

void ImportantMessagesWidget::removeOldest(int num)
{
	if (num >= d->labelWidgets.size())
	{
		clear();
	}
	else
	{
		for (; num > 0; --num)
		{
			removeOneOldest();
		}
	}
}

void ImportantMessagesWidget::removeOneOldest()
{
	if (!d->labelWidgets.isEmpty())
	{
		delete d->labelWidgets.takeFirst().pLabel;

		if (d->labelWidgets.isEmpty())
		{
			// Auto-hide when there is nothing to show.
			hide();
		}
	}
}

unsigned ImportantMessagesWidget::maxMessages() const
{
	return d->maxMessages;
}

void ImportantMessagesWidget::setMaxMessages(unsigned num)
{
	d->maxMessages = num;
}
