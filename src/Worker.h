#pragma once

#include "Observer.h"

class Cache;
class WeatherGetter;
class GeoCoordinates;

class IWorker : public IObservable
{
public:

	virtual ~IWorker() = default;

public:

	virtual void Run(const GeoCoordinates& coords) = 0;

};

class Worker : public IWorker
{
public:

	virtual ~Worker() override = default;

	Worker(const std::shared_ptr<Cache>& cache, const std::shared_ptr<WeatherGetter>& weatherGetter, std::atomic_bool& stopEvent);

public:

	virtual void Run(const GeoCoordinates& coords) override;

private:

	void CheckStopEvent();

private:

	const std::shared_ptr<Cache> m_cache;
	
	const std::shared_ptr<WeatherGetter> m_weatherGetter;

	std::atomic_bool& m_stopEvent;

};