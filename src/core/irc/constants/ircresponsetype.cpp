//------------------------------------------------------------------------------
// ircresponsetype.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircresponsetype.h"

IRCResponseType::IRCResponseType()
{
	d.type = Invalid;
	d.numericType = -1;
}

IRCResponseType::IRCResponseType(MsgType type)
{
	d.type = type;
	d.numericType = toRfcNumber(type);
}

IRCResponseType::IRCResponseType(const QString& type)
{
	d.type = typeFromRfcString(type);

	// Get numeric type from string.
	bool bOk = false;
	int numericType = type.toInt(&bOk);
	if (!bOk)
	{
		numericType = -1;
	}
	d.numericType = numericType;
}

IRCResponseType IRCResponseType::fromIntegerResponseValue(int responseType)
{
	// Response type is always a 3-digit number. Conver the passed integer with
	// leading zeros if necessary and then pass it to typeFromRfcString().

	// The result here is d.numericType always set to responseType, and the
	// d.type set to either Invalid or appropriate value if the number was
	// recognized.
	QString strResponseType = QString("%1").arg(responseType, 3, 10, QChar('0'));

	IRCResponseType newIRCResponseType;
	newIRCResponseType.d.type = typeFromRfcString(strResponseType);
	newIRCResponseType.d.numericType = responseType;

	return newIRCResponseType;
}

bool IRCResponseType::operator==(const IRCResponseType& other) const
{
	return d.type == other.d.type;
}

bool IRCResponseType::operator!=(const IRCResponseType& other) const
{
	return !((*this) == other);
}

int IRCResponseType::toRfcNumber(MsgType type)
{
	if (type == Invalid)
	{
		return -1;
	}

	QString str = toRfcString(type);

	bool bOk = false;
	int val = str.toInt(&bOk);

	return bOk ? val : -1;
}

QString IRCResponseType::toRfcString(MsgType type)
{
	switch (type)
	{
		case HelloClient:
			return "001";

		case RPLLUserClient:
			return "251";

		case RPLLUserOp:
			return "252";

		case RPLLUserUnknown:
			return "253";

		case RPLLUserChannels:
			return "254";

		case RPLLUserMe:
			return "255";

		case RPLWhoIsUser:
			return "311";

		case RPLNamReply:
			return "353";

		case RPLEndOfNames:
			return "366";

		case RPLMOTD:
			return "372";

		case RPLMOTDStart:
			return "375";

		case RPLEndOfMOTD:
			return "376";

		case ERRNoSuchNick:
			return "401";

		case ERRNicknameInUse:
			return "433";

		case ERRChanOpPrivIsNeeded:
			return "482";

		case Join:
			return "JOIN";

		case Kick:
			return "KICK";

		case Kill:
			return "KILL";

		case Mode:
			return "MODE";

		case Nick:
			return "NICK";

		case Notice:
			return "NOTICE";

		case Part:
			return "PART";

		case Ping:
			return "PING";

		case PrivMsg:
			return "PRIVMSG";

		case Quit:
			return "QUIT";

		case Invalid:
		default:
			return QString();
	}
}

IRCResponseType::MsgType IRCResponseType::typeFromRfcString(const QString& typeRepresentation)
{
	QString strTypeUpper = typeRepresentation.trimmed().toUpper();

	// Compare passed string against all known types.
	for (int i = 0; i < NUM_TYPES; ++i)
	{
		MsgType enumType = (MsgType) i;
		QString strCurrent = toRfcString(enumType);
		if (strTypeUpper == strCurrent)
		{
			return enumType;
		}
	}

	return Invalid;
}
