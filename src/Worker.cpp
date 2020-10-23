#include "pch.h"
#include "Worker.h"
#include "Cache.h"
#include "CacheStorage.h"
#include "GeoCoordinates.h"
#include "Weather.h"
#include "WeatherGetter.h"

class StopException : public std::exception
{
public:
	using std::exception::exception;
};

Worker::Worker(const std::shared_ptr<Cache>& cache, const std::shared_ptr<WeatherGetter>& weatherGetter, std::atomic_bool& stopEvent)
	: m_cache(cache)
	, m_weatherGetter(weatherGetter)
	, m_stopEvent(stopEvent)
{
}

void Worker::Run(const GeoCoordinates& coords)
{
	try
	{
		CheckStopEvent();
		const auto cacheWeather = m_cache->GetItem(coords);
		if (!cacheWeather.has_value())
		{
			CheckStopEvent();
			const auto calcedWeather = m_weatherGetter->Get(coords);
			m_cache->AddItem(coords, calcedWeather);

			CheckStopEvent();
			NotifyResult(MessageType::Calced, coords, calcedWeather);
		}
		else
		{
			CheckStopEvent();
			NotifyResult(MessageType::FoundInCache, coords, cacheWeather.value());
		}
	}
	catch (StopException&)
	{
		return;
	}
	catch (std::exception& ex)
	{
		NotifyError(ex.what());
	}
}

void Worker::CheckStopEvent()
{
	if (m_stopEvent)
		throw StopException();
}
