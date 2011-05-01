//------------------------------------------------------------------------------
// speedcalculator.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "speedcalculator.h"

SpeedCalculator::SpeedCalculator()
{
	dataSizeExpected = 0;
	_maxResolution = 50;
}

float SpeedCalculator::estimatedTimeUntilArrival() const
{
	float speed = getSpeed();
	if (speed == 0.0f)
	{
		return -1.0f;
	}

	const DataArrivalInfo& endInfo = arrivalData[arrivalData.size() - 1];
	unsigned currentData = endInfo.totalAmountOfArrivedData;

	// This happens if dataSizeExpected isn't set properly.
	if (dataSizeExpected < currentData)
	{
		return -1.0f;
	}

	unsigned remainingData = dataSizeExpected - currentData;
	return (float)remainingData / speed;
}

float SpeedCalculator::getSpeed() const
{
	int timeDifference = maxTimeDifference();
	if (timeDifference == 0)
	{
		return 0.0f;
	}

	float dataDifference = 0.0f;

	const DataArrivalInfo& beginInfo = arrivalData[0];
	const DataArrivalInfo& endInfo = arrivalData[arrivalData.size() - 1];

	dataDifference = endInfo.totalAmountOfArrivedData - beginInfo.totalAmountOfArrivedData;
	float speed = dataDifference / (float)timeDifference;

	// Scale speed to 1 second.
	speed *= 1000.0f;

	return speed;
}

unsigned SpeedCalculator::maxTimeDifference() const
{
	if (arrivalData.size() < 2)
	{
		return 0;
	}

	const DataArrivalInfo& beginInfo = arrivalData[0];
	const DataArrivalInfo& endInfo = arrivalData[arrivalData.size() - 1];

	return endInfo.timeOfArrival - beginInfo.timeOfArrival;
}

void SpeedCalculator::registerDataAmount(unsigned totalAmountOfArrivedData)
{
	DataArrivalInfo dataArrivalInfo(totalAmountOfArrivedData, clock.elapsed());
	arrivalData << dataArrivalInfo;
	if (arrivalData.size() > _maxResolution)
	{
		arrivalData.pop_front();
	}
}

void SpeedCalculator::setExpectedDataSize(unsigned size)
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
		arrivalData.pop_front();
	}
}

void SpeedCalculator::start()
{
	arrivalData.clear();
	dataSizeExpected = 0;
	clock.start();
}
