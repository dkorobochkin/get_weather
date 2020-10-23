#pragma once

#include "GeoCalculator.h"
#include "GeoCoordinates.h"

class GeoCalculatorMock : public IGeoCalculator
{
public:
	MOCK_CONST_METHOD2(CalcDistanceInMeters, double(const GeoCoordinates&, const GeoCoordinates&));
	MOCK_CONST_METHOD0(GetDistanceError, double());
};
