//------------------------------------------------------------------------------
// importantmessageswidget.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "importantmessageswidget.h"

#include <QDateTime>

ImportantMessagesWidget::ImportantMessagesWidget(QWidget* pParent)
: QWidget(pParent)
{
	_maxMessages = DEFAULT_MAX_MESSAGES;

	pLayout = new QVBoxLayout();
	pLayout->setSpacing(1);
	pLayout->setMargin(0);
	pLayout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(pLayout);

	this->setContentsMargins(0, 0, 0, 0);

	// Setup the clean up timer.
	this->connect(&oldLabelCleaner, SIGNAL( timeout() ),
		SLOT( dropOldWidgetsIfBeyondLimit()) );

	// Remember to convert seconds to msecs.
	oldLabelCleaner.start(MAX_MSG_KEEP_TIME_SEC * 1000);
	this->hide();
}

ImportantMessagesWidget::~ImportantMessagesWidget()
{
	delete pLayout;
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

	QLabel* pNewLabel = new QLabel(this);
	pNewLabel->setText(strTimestamp + message);

	labelWidgets << MessageLabel(pNewLabel);
	pLayout->addWidget(pNewLabel);

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
	while (!labelWidgets.isEmpty())
	{
		removeOneOldest();
	}
}

#include <cstdio>

void ImportantMessagesWidget::dropOldWidgetsIfBeyondLimit()
{
	while (labelWidgets.size() > _maxMessages)
	{
		MessageLabel& oldestLabel = labelWidgets.first();
		int timeDifference = oldestLabel.timeCreated.secsTo(QDateTime::currentDateTime());

		if (timeDifference > MAX_MSG_KEEP_TIME_SEC)
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
	if (num >= labelWidgets.size())
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
	if (!labelWidgets.isEmpty())
	{
		delete labelWidgets.takeFirst().pLabel;

		if (labelWidgets.isEmpty())
		{
			// Auto-hide when there is nothing to show.
			//this->hide();
		}
	}
}
