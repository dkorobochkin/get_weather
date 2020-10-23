#pragma once

class GeoCoordinates;
class Weather;

class WeatherGetter
{
public:

	virtual ~WeatherGetter() = default;

public:

	virtual Weather Get(const GeoCoordinates& coordinates) const = 0;

};
