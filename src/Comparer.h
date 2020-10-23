#pragma once

class GeoCoordinates;

class Comparer
{
public:

	virtual ~Comparer() = default;

public:

	virtual bool AreEqual(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const = 0;

};
