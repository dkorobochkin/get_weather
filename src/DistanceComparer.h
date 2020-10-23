#pragma once

#include "Comparer.h"

class IGeoCalculator;

class DistanceComparer : public Comparer
{
public:

	DistanceComparer(double distanceDeltaInMeters, const std::shared_ptr<IGeoCalculator>& calculator);

	virtual ~DistanceComparer() override = default;

public:

	virtual bool AreEqual(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const override;

private:

	const double m_distanceDeltaMeters;

	const std::shared_ptr<IGeoCalculator> m_calculator;

};
