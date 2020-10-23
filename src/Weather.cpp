#include "pch.h"
#include "Weather.h"
#include "StringUtils.h"

namespace WeatherKeys
{
	static const std::string main = "main";
	static const std::string temp = "temp";
	static const std::string pressure = "pressure";
	static const std::string humidity = "humidity";
	static const std::string wind = "wind";
	static const std::string speed = "speed";
	static const std::string gust = "gust";
	static const std::string clouds = "clouds";
	static const std::string cloudsAll = "all";
}

Weather::Weather(const nlohmann::json& weatherJson)
	: m_weatherJson(weatherJson)
{
}

const nlohmann::json& Weather::GetWeatherJson() const
{
	return m_weatherJson;
}

std::string Weather::ToString() const
{
	std::string result = "Weather:";

	if (m_weatherJson.contains(WeatherKeys::main))
	{
		const auto main = m_weatherJson.at(WeatherKeys::main);

		if (main.contains(WeatherKeys::temp))
			result += "\n  Temperature: " + StringUtils::DoubleToString(main.at(WeatherKeys::temp).get<double>(), 2) + " C";
		if (main.contains(WeatherKeys::pressure))
			result += "\n  Atmospheric pressure: " + StringUtils::DoubleToString(main.at(WeatherKeys::pressure).get<double>(), 2) + " hPa";
		if (main.contains(WeatherKeys::humidity))
			result += "\n  Humidity: " + std::to_string(main.at(WeatherKeys::humidity).get<int>()) + " %";
	}

	if (m_weatherJson.contains(WeatherKeys::wind))
	{
		const auto wind = m_weatherJson.at(WeatherKeys::wind);

		if (wind.contains(WeatherKeys::speed))
			result += "\n  Wind speed: " + StringUtils::DoubleToString(wind.at(WeatherKeys::speed).get<double>(), 2) + " m/s";
		if (wind.contains(WeatherKeys::gust))
			result += "\n  Wind gust: " + StringUtils::DoubleToString(wind.at(WeatherKeys::gust).get<double>(), 2) + " m/s";
	}

	if (m_weatherJson.contains(WeatherKeys::clouds))
	{
		const auto clouds = m_weatherJson.at(WeatherKeys::clouds);

		if (clouds.contains(WeatherKeys::cloudsAll))
			result += "\n  Cloudiness: " + std::to_string(clouds.at(WeatherKeys::cloudsAll).get<int>()) + " %";
	}

	return result;
}

bool Weather::operator==(const Weather& other) const
{
	return m_weatherJson == other.m_weatherJson;
}

bool Weather::operator!=(const Weather& other) const
{
	return m_weatherJson != other.m_weatherJson;
}
