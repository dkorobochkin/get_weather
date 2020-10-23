#pragma once

#include "WeatherGetter.h"
#include "GeoCoordinates.h"
#include "Weather.h"

class WeatherGetterMock : public WeatherGetter
{
public:
	MOCK_CONST_METHOD1(Get, Weather(const GeoCoordinates&));
};
