#include "pch.h"
#include "TimeCache.h"
#include "Comparer.h"
#include "GeoCoordinates.h"
#include "Weather.h"

namespace JsonKeys
{
	static const std::string time = "time";
	static const std::string coordinates = "coord";
	static const std::string lat = "lat";
	static const std::string lon = "lon";
	static const std::string weather = "weather";
	static const std::string expirationTime = "exp_time";
	static const std::string values = "values";
	static const std::string version = "version";
	static const std::string cacheType = "cache_type";
}

namespace JsonValues
{
	constexpr int currentVersion = 1;
	static const std::string cacheType = "time";
}

TimeCache::TimeCache(std::chrono::minutes expirationTime, const std::shared_ptr<Comparer>& comparer, const std::shared_ptr<IChronoUtils>& chronoUtils)
	: m_expirationTime(expirationTime)
	, m_comparer(comparer)
	, m_chronoUtils(chronoUtils)
{
}

void TimeCache::AddItem(const GeoCoordinates& coordinates, const Weather& weather)
{
	std::lock_guard<std::mutex> lg(m_mutex);

	const auto curTime = m_chronoUtils->CurrentTime();
	RemoveExpired(curTime);
	Insert(curTime, coordinates, weather);
}

std::optional<Weather> TimeCache::GetItem(const GeoCoordinates& coordinates) const
{
	std::lock_guard<std::mutex> lg(m_mutex);

	const auto validTime = GetMinValidTime(m_chronoUtils->CurrentTime());
	const auto foundIt = std::find_if(std::begin(m_values), std::end(m_values),
		[&](const auto& item)
		{
			if (item.first >= validTime)
				return m_comparer->AreEqual(item.second.first, coordinates);
			return false;
		});

	if (foundIt != std::end(m_values))
		return foundIt->second.second;

	return std::nullopt;
}

void TimeCache::RemoveExpired(const Time currentTime)
{
	if (m_values.empty())
		return;

	m_values.erase(m_values.lower_bound(GetMinValidTime(currentTime)), std::end(m_values));
}

Time TimeCache::GetMinValidTime(const Time currentTime) const
{
	return currentTime - m_expirationTime;
}

void TimeCache::Insert(const Time key, const GeoCoordinates& coordinates, const Weather& weather)
{
	m_values.emplace(key, CacheValue(coordinates, weather));
}

std::string TimeCache::Serialize() const
{
	std::lock_guard<std::mutex> lg(m_mutex);

	try
	{
		auto items = nlohmann::json::array();
		for (const auto& item : m_values)
		{
			const nlohmann::json obj = 
			{
				{ JsonKeys::time, Clock::to_time_t(item.first) },
				{ JsonKeys::coordinates, 
					{
						{ JsonKeys::lat, item.second.first.GetLatitudeInDegrees() },
						{ JsonKeys::lon, item.second.first.GetLongitudenInDegrees() }
					} 
				},
				{ JsonKeys::weather, item.second.second.GetWeatherJson() }
			};

			items.push_back(obj);
		}

		auto content = nlohmann::json::object();
		content[JsonKeys::expirationTime] = m_expirationTime.count();
		content[JsonKeys::values] = items;
		content[JsonKeys::version] = JsonValues::currentVersion;
		content[JsonKeys::cacheType] = JsonValues::cacheType;

		return content.dump();
	}
	catch (std::exception&)
	{
	}

	return {};
}

bool TimeCache::Deserialize(const std::string& buffer)
{
	std::lock_guard<std::mutex> lg(m_mutex);

	bool ret = true;

	try
	{
		const auto content = nlohmann::json::parse(buffer);
		if (IsValidJson(content))
		{
			if (content.at(JsonKeys::version).get<int>() != JsonValues::currentVersion ||
				content.at(JsonKeys::cacheType).get<std::string>() != JsonValues::cacheType)
			{
				return false;
			}

			const auto items = content.at(JsonKeys::values);
			for (const auto& item : items)
			{
				try
				{
					if (item.contains(JsonKeys::time) && item.contains(JsonKeys::coordinates) && item.contains(JsonKeys::weather))
					{
						const auto coordinates = item.at(JsonKeys::coordinates);
						if (!coordinates.contains(JsonKeys::lat) && coordinates.contains(JsonKeys::lon))
							continue;

						const auto lat = coordinates.at(JsonKeys::lat).get<double>();
						const auto lon = coordinates.at(JsonKeys::lon).get<double>();
						const auto time = item.at(JsonKeys::time).get<time_t>();
						const auto weatherJson = item.at(JsonKeys::weather);

						const GeoCoordinates geoCoords(lat, lon);
						const Weather weather(weatherJson);

						Insert(Clock::from_time_t(time), geoCoords, weather);
					}
				}
				catch (std::exception&)
				{
				}
			}

			const auto expirationTime = std::chrono::minutes(content.at(JsonKeys::expirationTime).get<int>());
			SetExpirationTime(expirationTime);
		}
		else
		{
			ret = false;
		}
	}
	catch (std::exception&)
	{
		ret = false;
	}

	return ret;
}

void TimeCache::SetExpirationTime(std::chrono::minutes expirationTime)
{
	m_expirationTime = expirationTime;
	RemoveExpired(m_chronoUtils->CurrentTime());
}

bool TimeCache::IsValidJson(const nlohmann::json& json) const
{
	return json.contains(JsonKeys::expirationTime) &&
		json.contains(JsonKeys::values) &&
		json.contains(JsonKeys::cacheType) &&
		json.contains(JsonKeys::version);
}
