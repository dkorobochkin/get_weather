#pragma once

#include "Cache.h"
#include "GeoCoordinates.h"
#include "Weather.h"

class CacheMock : public Cache
{
public:
	MOCK_METHOD2(AddItem, void(const GeoCoordinates&, const Weather&));
	MOCK_CONST_METHOD1(GetItem, std::optional<Weather>(const GeoCoordinates&));

	MOCK_CONST_METHOD0(Serialize, std::string());
	MOCK_METHOD1(Deserialize, bool(const std::string& buffer));
};

