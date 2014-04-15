//------------------------------------------------------------------------------
// ircnicknamecompleter.cpp
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircnicknamecompleter.h"

#include "gui/irc/ircuserlistmodel.h"

#include <QCompleter>
#include <QRegExp>

class IRCNicknameCompleter::State
{
	public:
		int cursorPos;
		QString textLine;

		State()
		{
			cursorPos = -1;
		}

		QString extractNicknamePrefix() const
		{
			const QString NICKNAME_MATCH = "\\[\\]\\{\\}\\-\\^\\`\\|\\\\A-Za-z0-9_";
			QRegExp regex(QString("(?:.*)[^%1]?([%1]*)").arg(NICKNAME_MATCH));
			regex.setCaseSensitivity(Qt::CaseInsensitive);
			regex.setMinimal(false);
			regex.indexIn(leftText());
			return regex.cap(1);
		}

		bool isValid() const
		{
			return cursorPos >= 0;
		}

		QString leftText() const
		{
			return textLine.left(cursorPos);
		}

		QString leftTextMinusNicknamePrefix() const
		{
			return textLine.left(cursorPos - extractNicknamePrefix().length());
		}

		QString rightText() const
		{
			return textLine.mid(cursorPos);
		}
};

class IRCNicknameCompleter::PrivData
{
	public:
		QCompleter completer;
		IRCNicknameCompleter::State state;

		IRCCompletionResult complete()
		{
			IRCCompletionResult result;
			if (!completer.currentCompletion().isEmpty())
			{
				result.textLine = leftTextWithCompletedNickname() + state.rightText();
				result.cursorPos = leftTextWithCompletedNickname().length();
			}
			return result;
		}

		QString leftTextWithCompletedNickname() const
		{
			return state.leftTextMinusNicknamePrefix() + completer.currentCompletion();
		}
};

IRCNicknameCompleter::IRCNicknameCompleter()
{
	d = new PrivData();
	d->completer.setCaseSensitivity(Qt::CaseInsensitive);
	d->completer.setCompletionRole(IRCUserListModel::RoleCleanNickname);
}

IRCNicknameCompleter::~IRCNicknameCompleter()
{
	delete d;
}

IRCCompletionResult IRCNicknameCompleter::complete(const QString &textLine,
	int cursorPosition)
{
	d->state = State();
	d->state.cursorPos = cursorPosition;
	d->state.textLine = textLine;

	QString nickPrefix = d->state.extractNicknamePrefix();
	d->completer.setCompletionPrefix(nickPrefix);
	return d->complete();
}

IRCCompletionResult IRCNicknameCompleter::cycleNext()
{
	if (d->completer.completionCount() == 0)
	{
		return IRCCompletionResult();
	}
	if (d->completer.currentRow() + 1 >= d->completer.completionCount())
	{
		d->completer.setCurrentRow(0);
	}
	else
	{
		d->completer.setCurrentRow(d->completer.currentRow() + 1);
	}
	return d->complete();
}

bool IRCNicknameCompleter::isReset() const
{
	return !d->state.isValid();
}

void IRCNicknameCompleter::reset()
{
	d->state = State();
}

void IRCNicknameCompleter::setModel(QAbstractItemModel *model)
{
	d->completer.setModel(model);
}
