#include "pch.h"
#include "GeoCoordinates.h"
#include "MathUtils.h"

namespace {
	constexpr double MinLatitude = -90.0;
	constexpr double MaxLatitude = 90.0;
	constexpr double MinLongitude = -180.0;
	constexpr double MaxLongitude = 180.0;
	constexpr double RadiansRatio = M_PI / 180.0;
}

GeoCoordinates::GeoCoordinates(double latitudeInDegrees, double longitudeInDegrees)
	: m_latitudeInDegrees(latitudeInDegrees)
	, m_longitudeInDegrees(longitudeInDegrees)
	, m_latitudeInRadians(latitudeInDegrees * RadiansRatio)
	, m_longitudeInRadians(longitudeInDegrees* RadiansRatio)
{
	if (m_latitudeInDegrees < MinLatitude || m_latitudeInDegrees > MaxLatitude)
		throw std::exception("Incorrect value of latitude");
	if (m_longitudeInDegrees < MinLongitude || m_longitudeInDegrees > MaxLongitude)
		throw std::exception("Incorrect value of longitude");
}

double GeoCoordinates::GetLatitudeInDegrees() const
{
	return m_latitudeInDegrees;
}

double GeoCoordinates::GetLongitudenInDegrees() const
{
	return m_longitudeInDegrees;
}

double GeoCoordinates::GetLatitudeInRadians() const
{
	return m_latitudeInRadians;
}

double GeoCoordinates::GetLongitudenInRadians() const
{
	return m_longitudeInRadians;
}

bool GeoCoordinates::operator==(const GeoCoordinates& other) const
{
	return MathUtils::Compare(m_latitudeInDegrees, other.m_latitudeInDegrees) &&
		MathUtils::Compare(m_longitudeInDegrees, other.m_longitudeInDegrees);
}

bool GeoCoordinates::operator!=(const GeoCoordinates& other) const
{
	return !MathUtils::Compare(m_latitudeInDegrees, other.m_latitudeInDegrees) ||
		!MathUtils::Compare(m_longitudeInDegrees, other.m_longitudeInDegrees);
}

std::string GeoCoordinates::ToString() const
{
	return "Geo coordinates:\n  Lat=" + std::to_string(m_latitudeInDegrees) + "\n  Lon=" + std::to_string(m_longitudeInDegrees);
}
