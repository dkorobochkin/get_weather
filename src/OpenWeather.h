#pragma once

#include "WeatherGetter.h"

class Connection;

class OpenWeather : public WeatherGetter
{
public:

	OpenWeather(const std::shared_ptr<Connection>& connection, const std::string& apiId);

	virtual ~OpenWeather() override = default;

public:

	virtual Weather Get(const GeoCoordinates & coordinates) const override;

protected:

	std::string PrepareUrl(const GeoCoordinates& coordinates) const;

private:

	void CheckReturnCode(const nlohmann::json& response) const;

private:

	const std::shared_ptr<Connection> m_connection;

	const std::string m_apiId;

};
