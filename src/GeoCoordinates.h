#pragma once

#include "Printable.h"

class GeoCoordinates : public IPrintable
{
public:

	GeoCoordinates(double latitudeInDegrees, double longitudeInDegrees);

	virtual ~GeoCoordinates() = default;

public:

	double GetLatitudeInDegrees() const;

	double GetLongitudenInDegrees() const;

	double GetLatitudeInRadians() const;

	double GetLongitudenInRadians() const;

	bool operator==(const GeoCoordinates& other) const;

	bool operator!=(const GeoCoordinates& other) const;

	virtual std::string ToString() const override;

private:

	double m_latitudeInDegrees;

	double m_longitudeInDegrees;

	double m_latitudeInRadians;

	double m_longitudeInRadians;

};
