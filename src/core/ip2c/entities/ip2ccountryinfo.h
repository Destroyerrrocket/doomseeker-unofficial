//------------------------------------------------------------------------------
// ip2ccountryinfo.h
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IP2CCOUNTRYINFO_H__
#define __IP2CCOUNTRYINFO_H__

#include "global.h"

#include <QPixmap>
#include <QString>

/**
 *	@brief Flag and name of the country.
 */
class MAIN_EXPORT IP2CCountryInfo
{
    public:
        const QPixmap*	flag;
        QString			name;

        IP2CCountryInfo();
        IP2CCountryInfo(const QPixmap* flag, QString name);

        bool isValid() const;
};

#endif
