#include "pch.h"
#include "FileCacheStorage.h"
#include "Cache.h"
#include "GeoCoordinates.h"
#include "Weather.h"

FileCacheStorage::FileCacheStorage(const std::wstring& path)
	: m_path(path)
{
}

bool FileCacheStorage::Read(Cache& cache) const
{
	try
	{
		std::string fileContent;
		if (std::filesystem::exists(m_path))
		{
			std::ifstream file(m_path);
			if (!file.is_open())
				return false;

			std::string str;
			while (file >> str)
				fileContent += str;
		}

		return cache.Deserialize(fileContent);
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return false;
}

bool FileCacheStorage::Write(const Cache& cache) const
{
	try
	{
		std::ofstream file(m_path, std::ios::out);
		if (!file.is_open())
			return false;

		file << cache.Serialize();
		return true;
	}
	catch (std::exception&)
	{
	}

	return false;
}

bool FileCacheStorage::Remove() const
{
	std::error_code ec;
	return (std::filesystem::exists(m_path, ec)) ? std::filesystem::remove(m_path, ec) : true;
}
