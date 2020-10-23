#pragma once

#include "Serializable.h"

class GeoCoordinates;
class Weather;

class Cache : public ISerializable
{
public:

	virtual ~Cache() override = default;

public:

	virtual void AddItem(const GeoCoordinates& coordinates, const Weather& weather) = 0;

	virtual std::optional<Weather> GetItem(const GeoCoordinates& coordinates) const = 0;

};
