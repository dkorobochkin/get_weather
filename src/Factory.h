#pragma once

class IGeoCalculator;
class IChronoUtils;
class IWorker;
class Connection;
class WeatherGetter;
class Comparer;
class Cache;
class CacheStorage;
class IObserver;

class Factory
{
public:

	Factory(double distanceDeltaInMeters,
		const std::wstring& pathToCacheStorage,
		const std::string& apiId,
		std::chrono::minutes expirationTime);

public:

	std::shared_ptr<Cache> GetCache() const;

	std::shared_ptr<CacheStorage> GetCacheStorage() const;

	std::shared_ptr<IWorker> CreateWorker(std::atomic_bool& stopEvent) const;

private:

	const std::shared_ptr<IGeoCalculator> m_geoCalculator;

	const std::shared_ptr<Connection> m_connection;

	const std::shared_ptr<WeatherGetter> m_weatherGetter;

	const std::shared_ptr<Comparer> m_comparer;

	const std::shared_ptr<IChronoUtils> m_chronoUtils;

	const std::shared_ptr<Cache> m_cache;

	const std::shared_ptr<CacheStorage> m_cacheStorage;

	const std::shared_ptr<IObserver> m_printer;

};
