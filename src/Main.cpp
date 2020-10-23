#include "pch.h"#include "Factory.h"
#include "Cache.h"
#include "CmdParser.h"
#include "GeoCoordinates.h"
#include "AppOptions.h"
#include "FileCacheStorage.h"
#include "Factory.h"
#include "GeoCalculator.h"
#include "Connection.h"
#include "WeatherGetter.h"
#include "Comparer.h"
#include "ChronoUtils.h"
#include "Worker.h"

namespace
{
	static const std::wstring localCachePath = L"cache";
	constexpr double sameDistanceInMeters = 30000.0;
	constexpr std::chrono::minutes expirationTime = 30min;

	std::atomic_bool stopEvent = false;
}

void Usage(const char* programName);
void SubscribeToCtrlCEvent();

int main(int argc, char** argv)
{
	AppOptions appOpt;
	if (!appOpt.Parse(argc, argv))
	{
		Usage(argv[0]);
		return 1;
	}

	switch (appOpt.GetAppMode())
	{
	case AppMode::Help:
		Usage(argv[0]);
		return 0;
	case AppMode::Clean:
	{
		FileCacheStorage storage(localCachePath);
		if (!storage.Remove())
		{
			std::cerr << "Can't clear local cache" << std::endl;
			return 2;
		}
		return 0;
	}
	case AppMode::Calculation:
	{
		try
		{
			SubscribeToCtrlCEvent();

			const auto apiId = appOpt.GetApiId();
			auto factory = std::make_unique<Factory>(sameDistanceInMeters, localCachePath, apiId, expirationTime);

			auto storage = factory->GetCacheStorage();
			auto cache = factory->GetCache();

			if (!storage->Read(*cache))
				std::cerr << "Can't read local cache" << std::endl << std::endl;

			std::cout << "Press Ctrl+C to cancell" << std::endl << std::endl;

			std::vector<std::thread> threads;
			for (auto coords : appOpt.GetCoordinates())
			{
				try
				{
					auto worker = factory->CreateWorker(stopEvent);
					threads.emplace_back([worker = std::move(worker), coords = std::move(coords)] {
						return worker->Run(coords);
					});
				}
				catch (std::exception& ex)
				{
					std::cerr << ex.what() << std::endl;
				}
			}
			for (auto& t : threads)
				t.join();

			if (!storage->Write(*cache))
				std::cerr << "Can't write cache to file" << std::endl << std::endl;
		}
		catch (std::exception & ex)
		{
			std::cerr << ex.what() << std::endl;
			return 3;
		}

		if (stopEvent)
			std::cout << "Process was break..." << std::endl;

		break;
	}
	default:
		Usage(argv[0]);
		return 3;
	}

	return 0;
}

void Usage(const char* programName)
{
	std::cout << "Usage: " << programName << " [--help] [--clear] --id <id> coordinates" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  --help, -h      Show this help" << std::endl;
	std::cout << "  --clear, -c     Clear local cache" << std::endl;
	std::cout << "  --id <api_id>   Existing API ID on https://openweathermap.org" << std::endl;
	std::cout << "Arguments:" << std::endl;
	std::cout << "  coordinates     List of latitudes and longitutes pairs" << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "  GetWeather --id b7689daa0c2 60.000359 30.201452" << std::endl;
	std::cout << "  GetWeather --clear" << std::endl;
	std::cout << "  GetWeather --help" << std::endl;
}

void StopEventWasReceived()
{
	std::cout << "Received stop signal" << std::endl;
	stopEvent = true;
}

#if defined(__linux__) || defined(__APPLE__)

void ConsoleHandler(sig_t s) 
{
	StopEventWasReceived();
}

void SubscribeToCtrlCEvent()
{
	signal(SIGINT, ConsoleHandler);
}

#elif _WIN32

bool ConsoleHandler(int signal) 
{
	if (signal == CTRL_C_EVENT)
		StopEventWasReceived();
	return true;
}

void SubscribeToCtrlCEvent()
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);
}

#endif
