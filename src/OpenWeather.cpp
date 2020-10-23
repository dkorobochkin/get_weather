#include "pch.h"
#include "OpenWeather.h"
#include "Connection.h"
#include "GeoCoordinates.h"
#include "Weather.h"

namespace OpenWeatherApi
{
	static const std::string url = "api.openweathermap.org/data/2.5/weather?";
	static const std::string lat = "lat";
	static const std::string lon = "lon";
	static const std::string appid = "appid";
	static const std::string units = "units";
	static const std::string metric = "metric";
}

namespace OpenWeatherResponse
{
	static const std::string returnCode = "cod";

	constexpr int incorrectApiKey = 401;
	constexpr int incorrectRequest = 404;
	constexpr int manyRequests = 429;
}

OpenWeather::OpenWeather(const std::shared_ptr<Connection>& connection, const std::string& apiId)
	: m_connection(connection)
	, m_apiId(apiId)
{
}

Weather OpenWeather::Get(const GeoCoordinates& coordinates) const
{
	const auto response = nlohmann::json::parse(m_connection->Request(PrepareUrl(coordinates)));
	CheckReturnCode(response);

	return Weather(response);
}

std::string OpenWeather::PrepareUrl(const GeoCoordinates& coordinates) const
{
	const std::vector<std::pair<std::string, std::string>> params = 
	{ 
		{ OpenWeatherApi::lat, std::to_string(coordinates.GetLatitudeInDegrees()) },
		{ OpenWeatherApi::lon, std::to_string(coordinates.GetLongitudenInDegrees()) },
		{ OpenWeatherApi::appid, m_apiId },
		{ OpenWeatherApi::units, OpenWeatherApi::metric }
	};

	auto url = OpenWeatherApi::url;
	for (auto i = 0u; i < params.size(); ++i)
	{
		if (i > 0)
			url += "&";
		url += params[i].first + "=" + params[i].second;
	}

	return url;
}

void OpenWeather::CheckReturnCode(const nlohmann::json& response) const
{
	const static std::unordered_map<int, std::string> Errors
	{
		{ OpenWeatherResponse::incorrectApiKey, "Incorrect API key" },
		{ OpenWeatherResponse::incorrectRequest, "Incorrect request" },
		{ OpenWeatherResponse::manyRequests, "You have free tariff and make many API calls per minute" }
	};

	if (response.contains(OpenWeatherResponse::returnCode))
	{
		const auto code = response.at(OpenWeatherResponse::returnCode).get<int>();
		
		const auto it = Errors.find(code);
		if (it != Errors.end())
			throw std::exception(it->second.c_str(), code);
	}
}
