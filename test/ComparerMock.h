#pragma once

#include "Comparer.h"
#include "GeoCoordinates.h"

class ComparerMock : public Comparer
{
public:
	MOCK_CONST_METHOD2(AreEqual, bool(const GeoCoordinates&, const GeoCoordinates&));
};
