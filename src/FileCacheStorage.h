#pragma once

#include "CacheStorage.h"

class FileCacheStorage : public CacheStorage
{
public:

	FileCacheStorage(const std::wstring& path);

	virtual ~FileCacheStorage() override = default;

public:

	virtual bool Read(Cache& cache) const override;

	virtual bool Write(const Cache& cache) const override;

	virtual bool Remove() const override;

private:

	const std::wstring m_path;

};
