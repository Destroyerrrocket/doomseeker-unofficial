#include "localizationinfo.h"

bool LocalizationInfo::operator==(const LocalizationInfo& o2) const
{
	return countryCodeName == o2.countryCodeName
		&& localeName == o2.localeName
		&& niceName == o2.niceName;
}

QString LocalizationInfo::toString() const
{
	return QString("%1;%2;%3").arg(countryCodeName, localeName, niceName);
}
