#include "pch.h"
#include "DistanceComparer.h"
#include "GeoCoordinates.h"
#include "GeoCalculator.h"

DistanceComparer::DistanceComparer(double distanceDeltaMeters, const std::shared_ptr<IGeoCalculator>& calculator)
	: m_distanceDeltaMeters(distanceDeltaMeters * (1.0 + calculator->GetDistanceError()))
	, m_calculator(calculator)
{
}

bool DistanceComparer::AreEqual(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const
{
	return m_calculator->CalcDistanceInMeters(coordinates1, coordinates2) <= m_distanceDeltaMeters;
}
