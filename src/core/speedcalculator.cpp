//------------------------------------------------------------------------------
// speedcalculator.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "speedcalculator.h"

SpeedCalculator::SpeedCalculator()
{
	dataSizeExpected = 0;
	_maxResolution = 50;
}

long double SpeedCalculator::estimatedTimeUntilArrival() const
{
	long double speed = getSpeed();
	if (speed == 0.0)
	{
		return -1.0;
	}

	const DataArrivalInfo& endInfo = arrivalData.last();
	unsigned currentData = endInfo.totalAmountOfArrivedData;

	// This happens if dataSizeExpected isn't set properly.
	if (dataSizeExpected < currentData)
	{
		return -1.0;
	}

	unsigned remainingData = dataSizeExpected - currentData;
	return (double)remainingData / speed;
}

long double SpeedCalculator::getSpeed() const
{
    if (arrivalData.size() < 2)
    {
        return 0.0;
    }

    // Let's calculate weighted mean.
	long double numerator = 0.0;
	long double denominator = 0.0;

	const DataArrivalInfo* prevDataPacket = &arrivalData[0];
	for (int i = 1; i < arrivalData.size(); ++i)
	{
        const DataArrivalInfo& dataPacket = arrivalData[i];

        long double dataDiff = dataPacket.totalAmountOfArrivedData - prevDataPacket->totalAmountOfArrivedData;
        long double timeDiff = dataPacket.timeOfArrival - prevDataPacket->timeOfArrival;

        numerator += timeDiff * dataDiff;
        denominator += timeDiff;

        prevDataPacket = &dataPacket;
	}

	long double speed = numerator / denominator;

	// Scale speed to 1 second.
	speed *= 1000.0;

	return speed;
}

qint64 SpeedCalculator::lastRegisteredDataAmount() const
{
    if (arrivalData.isEmpty())
    {
        return 0;
    }

    return arrivalData.last().totalAmountOfArrivedData;
}

qint64 SpeedCalculator::maxTimeDifference() const
{
	if (arrivalData.size() < 2)
	{
		return 0;
	}

	const DataArrivalInfo& beginInfo = arrivalData.first();
	const DataArrivalInfo& endInfo = arrivalData.last();

	return endInfo.timeOfArrival - beginInfo.timeOfArrival;
}

void SpeedCalculator::registerDataAmount(qint64 totalAmountOfArrivedData)
{
	DataArrivalInfo dataArrivalInfo(totalAmountOfArrivedData, clock.elapsed());
	arrivalData << dataArrivalInfo;
	if (arrivalData.size() > _maxResolution)
	{
		arrivalData.pop_front();
	}
}

void SpeedCalculator::setExpectedDataSize(qint64 size)
{
	dataSizeExpected = size;
}

void SpeedCalculator::setMaxResolution(int max)
{
	if (max < 2)
	{
		max = 2;
	}
	_maxResolution = max;

	while (arrivalData.size() > max)
	{
		// Drop the oldest data if it doesn't fit the vector anymore.
		arrivalData.pop_front();
	}
}

void SpeedCalculator::start()
{
	arrivalData.clear();
	dataSizeExpected = 0;
	clock.start();
}
