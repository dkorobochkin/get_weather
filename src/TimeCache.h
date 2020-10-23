#pragma once

#include "Cache.h"
#include "ChronoUtils.h"

class Comparer;

class TimeCache : public Cache
{
public:

	TimeCache(std::chrono::minutes expirationTime, const std::shared_ptr<Comparer>& comparer, const std::shared_ptr<IChronoUtils>& chronoUtils);

	TimeCache(const TimeCache&) = delete;

	TimeCache& operator=(const TimeCache&) = delete;

	virtual ~TimeCache() override = default;

public:

	virtual void AddItem(const GeoCoordinates& coordinates, const Weather& temperature) override;

	virtual std::optional<Weather> GetItem(const GeoCoordinates& coordinates) const override;

	virtual std::string Serialize() const override;

	virtual bool Deserialize(const std::string& buffer) override;

protected:

	using CacheValue = std::pair<GeoCoordinates, Weather>;

protected:

	void RemoveExpired(const Time currentTime);

private:

	Time GetMinValidTime(const Time currentTime) const;

	void Insert(const Time key, const GeoCoordinates& coordinates, const Weather& temperature);

	void SetExpirationTime(std::chrono::minutes expirationTime);

	bool IsValidJson(const nlohmann::json& json) const;

protected:

	std::multimap<Time, CacheValue, std::greater<Time>> m_values;

private:

	std::chrono::minutes m_expirationTime;

	const std::shared_ptr<Comparer> m_comparer;

	const std::shared_ptr<IChronoUtils> m_chronoUtils;

	mutable std::mutex m_mutex;

};
