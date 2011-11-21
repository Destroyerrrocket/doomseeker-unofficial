//------------------------------------------------------------------------------
// ip2ccountryinfo.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ip2ccountryinfo.h"

IP2CCountryInfo::IP2CCountryInfo()
{
	flag = NULL;
}

IP2CCountryInfo::IP2CCountryInfo(const QPixmap* flag, QString name)
{
	this->flag = flag;
	this->name = name;
}

bool IP2CCountryInfo::isValid() const
{
	return flag != NULL
		&& !flag->isNull()
		&& !name.isEmpty();
}
