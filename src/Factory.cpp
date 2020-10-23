#include "pch.h"
#include "Factory.h"
#include "GeoCalculator.h"
#include "HttpRequestConnection.h"
#include "OpenWeather.h"
#include "DistanceComparer.h"
#include "ChronoUtils.h"
#include "TimeCache.h"
#include "FileCacheStorage.h"
#include "Worker.h"
#include "Observer.h"
#include "Printer.h"

Factory::Factory(double distanceDeltaInMeters, 
	const std::wstring& pathToCacheStorage,
	const std::string& apiId, 
	std::chrono::minutes expirationTime)
	: m_geoCalculator(std::make_shared<GeoCalculator>())
	, m_connection(std::make_shared<HttpRequestConnection>())
	, m_weatherGetter(std::make_shared<OpenWeather>(m_connection, apiId))
	, m_comparer(std::make_shared<DistanceComparer>(distanceDeltaInMeters, m_geoCalculator))
	, m_chronoUtils(std::make_shared<ChronoUtils>())
	, m_cache(std::make_shared<TimeCache>(expirationTime, m_comparer, m_chronoUtils))
	, m_cacheStorage(std::make_shared<FileCacheStorage>(pathToCacheStorage))
	, m_printer(std::make_shared<Printer>())
{
}

std::shared_ptr<Cache> Factory::GetCache() const
{
	return m_cache;
}

std::shared_ptr<CacheStorage> Factory::GetCacheStorage() const
{
	return m_cacheStorage;
}

std::shared_ptr<IWorker> Factory::CreateWorker(std::atomic_bool& stopEvent) const
{
	auto worker = std::make_shared<Worker>(m_cache, m_weatherGetter, stopEvent);
	
	worker->Subscribe(MessageType::Error, m_printer);
	worker->Subscribe(MessageType::Calced, m_printer);
	worker->Subscribe(MessageType::FoundInCache, m_printer);

	return worker;
}
