#pragma once

class Cache;

class CacheStorage
{
public:

	virtual ~CacheStorage() = default;

public:

	virtual bool Read(Cache &cache) const = 0;

	virtual bool Write(const Cache& cache) const = 0;

	virtual bool Remove() const = 0;

};
