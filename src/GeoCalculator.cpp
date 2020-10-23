#include "pch.h"
#include "GeoCalculator.h"
#include "GeoCoordinates.h"

namespace {
	constexpr double FormulaError = 0.005;
	constexpr double AverageEarthRadiusMeters = 6371009.0;
}

double GeoCalculator::CalcDistanceInMeters(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const
{
	return AverageEarthRadiusMeters * CalcCentralAngle(coordinates1, coordinates2);
}

double GeoCalculator::GetDistanceError() const
{
	return FormulaError;
}

double GeoCalculator::CalcCentralAngle(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const
{
	const auto dAlfa = std::fabs(coordinates1.GetLongitudenInRadians() - coordinates2.GetLongitudenInRadians());
	const auto cosDAlfa = std::cos(dAlfa);
	const auto sinDAlfa = std::sin(dAlfa);
	const auto cosFi1 = std::cos(coordinates1.GetLatitudeInRadians());
	const auto sinFi1 = std::sin(coordinates1.GetLatitudeInRadians());
	const auto cosFi2 = std::cos(coordinates2.GetLatitudeInRadians());
	const auto sinFi2 = std::sin(coordinates2.GetLatitudeInRadians());
	
	const auto sqr1 = cosFi2 * sinDAlfa;
	const auto sqr2 = cosFi1 * sinFi2 - sinFi1 * cosFi2 * cosDAlfa;
	const auto numerator = std::hypot(sqr1, sqr2);
	const auto denominator = sinFi1 * sinFi2 + cosFi1 * cosFi2 * cosDAlfa;

	auto result = std::atan(numerator / denominator);
	if (numerator < 0.0 && denominator < 0.0)
		result = -M_PI + result;
	else if (numerator > 0.0 && denominator < 0)
		result = M_PI + result;

	return result;
}
