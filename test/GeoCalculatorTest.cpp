#include "pch.h"

#include "GeoCalculator.h"
#include "GeoCoordinates.h"

TEST(GeoCalculator, CalcDistance_SamePoints)
{
	GeoCalculator calculator;

	const std::vector<double> latitudes = { 59.979769, 0.0, 90.0 };
	const std::vector<double> longitudes = { 30.273844, 0.0, 180.0 };
	const std::vector<double> factors = { -1.0, 1.0 };

	for (auto i = 0u; i < latitudes.size(); ++i)
	{
		for (auto j = 0u; j < factors.size(); ++j)
		{
			for (auto k = 0u; k < longitudes.size(); ++k)
			{
				for (auto l = 0u; l < factors.size(); ++l)
				{
					auto point = GeoCoordinates(latitudes[i] * factors[j], latitudes[k] * factors[l]);
					EXPECT_DOUBLE_EQ(0.0, calculator.CalcDistanceInMeters(point, point));
				}
			}
		}
	}

	for (auto i = 0u; i < latitudes.size(); ++i)
	{
		for (auto j = 0u; j < factors.size(); ++j)
		{
			auto p1 = GeoCoordinates(latitudes[i] * factors[j], 180.0);
			auto p2 = GeoCoordinates(latitudes[i] * factors[j], -180.0);
			EXPECT_GT(0.01, calculator.CalcDistanceInMeters(p1, p2));
		}
	}
}

TEST(GeoCalculator, CalcDistance_SmallDistance)
{
	GeoCalculator calculator;

	auto p1 = GeoCoordinates(60.039494, 30.069612);
	auto p2 = GeoCoordinates(59.999818, 30.202199);
	const auto distance = calculator.CalcDistanceInMeters(p1, p2);
	EXPECT_LE(8587.0, distance);
	EXPECT_GE(8588.0, distance);
}

TEST(GeoCalculator, CalcDistance_BigDistance)
{
	GeoCalculator calculator;

	auto p1 = GeoCoordinates(59.134038, 30.701865);
	auto p2 = GeoCoordinates(60.000056, 30.203038);
	auto distance = calculator.CalcDistanceInMeters(p1, p2);
	EXPECT_LE(100311.0, distance);
	EXPECT_GE(100312.0, distance);

	p1 = GeoCoordinates(63.06951, 48.50030);
	p2 = GeoCoordinates(-23.25221, 123.55889);
	distance = calculator.CalcDistanceInMeters(p1, p2);
	EXPECT_LE(11582397.0, distance);
	EXPECT_GE(11582398.0, distance);
}