#pragma once

class GeoCoordinates;

class IGeoCalculator
{
public:

	virtual ~IGeoCalculator() = default;

public:

	virtual double CalcDistanceInMeters(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const = 0;

	virtual double GetDistanceError() const = 0;

};

class GeoCalculator : public IGeoCalculator
{
public:

	virtual ~GeoCalculator() override = default;

public:

	virtual double CalcDistanceInMeters(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const override;

	virtual double GetDistanceError() const override;

private:

	double CalcCentralAngle(const GeoCoordinates& coordinates1, const GeoCoordinates& coordinates2) const;

};
