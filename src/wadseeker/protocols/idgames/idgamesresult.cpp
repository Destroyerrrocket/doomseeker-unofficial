//------------------------------------------------------------------------------
// idgamesresult.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "idgamesresult.h"

IdgamesResult::IdgamesResult(const QVariant &rawData)
{
	data = rawData.toMap();
}

QVariant IdgamesResult::content() const
{
	return data["content"];
}

QString IdgamesResult::errorKeyword() const
{
	return isError() ? "error" : "warning";
}

QString IdgamesResult::errorType() const
{
	return data[errorKeyword()].toMap()["type"].toString();
}

QString IdgamesResult::errorMessage() const
{
	return data[errorKeyword()].toMap()["message"].toString();
}

bool IdgamesResult::hasErrorMessage() const
{
	return isError() || isWarning();
}

bool IdgamesResult::isError() const
{
	return data.contains("error");
}

bool IdgamesResult::isNull() const
{
	return data.isEmpty();
}

bool IdgamesResult::isOk() const
{
	return data.contains("content");
}

bool IdgamesResult::isWarning() const
{
	return data.contains("warning");
}

IdgamesResult IdgamesResult::mkError(const QString &type, const QString &message)
{
	QVariantMap m;
	QVariantMap error;
	error["type"] = type;
	error["message"] = message;
	m["error"] = error;
	return IdgamesResult(m);
}
