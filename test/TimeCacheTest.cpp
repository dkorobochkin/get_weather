#include "pch.h"

#include "TimeCache.h"
#include "ComparerMock.h"
#include "ChronoUtilsMock.h"
#include "GeoCoordinates.h"
#include "Weather.h"

using ::testing::_;
using ::testing::Return;

class TimeCacheTestable : public TimeCache
{
public:
	using TimeCache::TimeCache;

	const auto& GetValues() const
	{
		return m_values;
	}

	void RemoveExpiredItems(const Time currentTime)
	{
		RemoveExpired(currentTime);
	}
};

class TimeCacheTest : public ::testing::Test
{
protected:

	virtual void SetUp() override
	{
		m_comparer = std::make_shared<ComparerMock>();
		m_chronoUtils = std::make_shared<ChronoUtilsMock>();
		m_testObj = std::make_shared<TimeCacheTestable>(m_expirationTime, m_comparer, m_chronoUtils);
	}

protected:

	std::shared_ptr<ComparerMock> m_comparer;

	std::shared_ptr<ChronoUtilsMock> m_chronoUtils;

	std::shared_ptr<TimeCacheTestable> m_testObj;

	std::chrono::minutes m_expirationTime = 30min;

};

TEST_F(TimeCacheTest, AddItem_One)
{
	const auto expLat = 1.0;
	const auto expLong = 1.0;
	const auto expWeatherJson = R"({"temp" : 20.1})"_json;

	const auto coord = GeoCoordinates(expLat, expLong);
	const auto weather = Weather(expWeatherJson);
	const auto time = Clock::now();

	EXPECT_CALL(*m_comparer, AreEqual(_, _))
		.Times(0);
	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.Times(1)
		.WillOnce(Return(time));

	m_testObj->AddItem(coord, weather);

	ASSERT_EQ(1u, m_testObj->GetValues().size());
	
	const auto& item = *m_testObj->GetValues().begin();
	const auto& key = item.first;
	const auto& coords = item.second.first;
	const auto& temperature = item.second.second;
	EXPECT_DOUBLE_EQ(expLat, coords.GetLatitudeInDegrees());
	EXPECT_DOUBLE_EQ(expLong, coords.GetLongitudenInDegrees());
	EXPECT_EQ(expWeatherJson, temperature.GetWeatherJson());
	EXPECT_EQ(time, key);
}

TEST_F(TimeCacheTest, AddItem_SeveralItems)
{
	const auto NUM_ITEMS = 5u;

	EXPECT_CALL(*m_comparer, AreEqual(_, _))
		.Times(0);

	auto time = Clock::now();	
	for (auto i = 0u; i < NUM_ITEMS; ++i)
	{
		const auto delta = 0.1;
		const auto expLat = 1.0 + i * delta;
		const auto expLong = 1.0 + i * delta;
		const auto expWeatherJson = nlohmann::json(R"("temp": )" + std::to_string(20.1f + static_cast<float>(i * delta)));

		const auto coord = GeoCoordinates(expLat, expLong);
		const auto weather = Weather(expWeatherJson);
		time += i * 1min;

		EXPECT_CALL(*m_chronoUtils, CurrentTime())
			.Times(1)
			.WillRepeatedly(Return(time));

		m_testObj->AddItem(coord, weather);

		ASSERT_EQ(i + 1u, m_testObj->GetValues().size());

		const auto& item = *m_testObj->GetValues().begin(); // An adding item is newer (by time) that other and it's addded to the beginning
		const auto& key = item.first;
		const auto& coords = item.second.first;
		const auto& temperature = item.second.second;
		EXPECT_DOUBLE_EQ(expLat, coords.GetLatitudeInDegrees());
		EXPECT_DOUBLE_EQ(expLong, coords.GetLongitudenInDegrees());
		EXPECT_EQ(expWeatherJson, temperature.GetWeatherJson());
		EXPECT_EQ(time, key);
	}
}

TEST_F(TimeCacheTest, GetItem_ExistingAndValidItem)
{
	const auto lat1 = 1.0, lat2 = 1.1;
	const auto long1 = 1.0, long2 = 1.1;
	const auto weatherJson1 = R"({"temp" : 20.1})"_json;

	const auto coord1 = GeoCoordinates(lat1, long1);
	const auto weather1 = Weather(weatherJson1);
	const auto time1 = Clock::now();

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.WillOnce(Return(time1));

	m_testObj->AddItem(coord1, weather1);

	const auto coord2 = GeoCoordinates(lat2, long2);

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.WillOnce(Return(time1 + 1min));
	EXPECT_CALL(*m_comparer, AreEqual(coord1, coord2))
		.Times(1)
		.WillOnce(Return(true));

	const auto foundItem = m_testObj->GetItem(coord2);

	EXPECT_NE(std::nullopt, foundItem);
	EXPECT_EQ(weatherJson1, foundItem->GetWeatherJson());
}

TEST_F(TimeCacheTest, GetItem_ItemIsInvalid)
{
	const auto lat1 = 1.0, lat2 = 1.1;
	const auto long1 = 1.0, long2 = 1.1;
	const auto weatherJson1 = R"({"temp": 20.1})"_json;

	const auto coord1 = GeoCoordinates(lat1, long1);
	const auto temp1 = Weather(weatherJson1);
	const auto time1 = Clock::now();

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.WillOnce(Return(time1));

	m_testObj->AddItem(coord1, temp1);

	const auto coord2 = GeoCoordinates(lat2, long2);

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.WillOnce(Return(time1 + m_expirationTime + 10min));
	EXPECT_CALL(*m_comparer, AreEqual(coord1, coord2))
		.Times(0);

	const auto foundItem = m_testObj->GetItem(coord2);

	EXPECT_EQ(std::nullopt, foundItem);
}

TEST_F(TimeCacheTest, RemoveExpiredItem)
{
	const auto NUM_ITEMS = 5u;

	EXPECT_CALL(*m_comparer, AreEqual(_, _))
		.Times(0);

	auto startTime = Clock::now();
	for (auto i = 1u; i <= NUM_ITEMS; ++i)
	{
		const auto delta = 0.1;
		const auto expLat = 1.0 + i * delta;
		const auto expLong = 1.0 + i * delta;
		const auto expTemp = nlohmann::json::parse("{\"temp\": " + std::to_string(i) + "}");
		const auto time = startTime + i * 4min;

		const auto coord = GeoCoordinates(expLat, expLong);
		const auto temp = Weather(expTemp);

		EXPECT_CALL(*m_chronoUtils, CurrentTime())
			.Times(1)
			.WillRepeatedly(Return(time));

		m_testObj->AddItem(coord, temp);
	}

	ASSERT_EQ(NUM_ITEMS, m_testObj->GetValues().size());

	m_testObj->RemoveExpiredItems(startTime + m_expirationTime + 14min);

	const auto& values = m_testObj->GetValues();
	ASSERT_EQ(2u, values.size());

	const std::array<double, 2> expectedLatitudes = { 1.5, 1.4 };
	const std::array<double, 2> expectedLongitudes = { 1.5, 1.4 };
	const std::array<nlohmann::json, 2> expectedTemperatures = { "{\"temp\": 5}"_json, "{\"temp\": 4}"_json };
	const std::array<Time, 2> expectedTimes = { startTime + 20min, startTime + 16min };

	auto i = 0u;
	for (const auto& item : values)
	{
		const auto& key = item.first;
		const auto& coords = item.second.first;
		const auto& temperature = item.second.second;
		EXPECT_DOUBLE_EQ(expectedLatitudes[i], coords.GetLatitudeInDegrees());
		EXPECT_DOUBLE_EQ(expectedLongitudes[i], coords.GetLongitudenInDegrees());
		EXPECT_EQ(expectedTemperatures[i], temperature.GetWeatherJson());
		EXPECT_EQ(expectedTimes[i], key);

		++i;
	}
}

TEST_F(TimeCacheTest, Serialize)
{
	const auto NUM_ITEMS = 2u;
	const auto delta = 0.1;

	EXPECT_CALL(*m_comparer, AreEqual(_, _))
		.Times(0);

	auto startTime = Clock::from_time_t(100);
	for (auto i = 0u; i < NUM_ITEMS; ++i)
	{	
		const auto lat = 1.0 + i * delta;
		const auto lon = 1.0 + i * delta;
		auto weather = nlohmann::json::object();
		weather["temp"] = i;
		const auto time = startTime + i * 1s;

		const auto coord = GeoCoordinates(lat, lon);
		const auto temp = Weather(weather);

		EXPECT_CALL(*m_chronoUtils, CurrentTime())
			.Times(1)
			.WillRepeatedly(Return(time));

		m_testObj->AddItem(coord, temp);
	}

	const auto expectedJson = R"(
{
	"exp_time" : 30,
	"version" : 1,
	"cache_type" : "time",
	"values" : [
		{
			"time" : 101,
			"coord" : {
				"lat" : 1.1,
				"lon" : 1.1
			},
			"weather" : {
				"temp" : 1
			}
		},
		{
			"time" : 100,
			"coord" : {
				"lat" : 1.0,
				"lon" : 1.0
			},
			"weather" : {
				"temp" : 0
			}
		}
	]
})"_json;
	const auto actualJson = nlohmann::json::parse(m_testObj->Serialize());

	EXPECT_EQ(expectedJson, actualJson);
}

TEST_F(TimeCacheTest, Deserialize)
{
	const auto json = R"(
{
	"exp_time" : 30,
	"version" : 1,
	"cache_type" : "time",
	"values" : [
		{
			"time" : 101,
			"coord" : {
				"lat" : 1.1,
				"lon" : 1.1
			},
			"weather" : {
				"temp" : 1
			}
		},
		{
			"time" : 100,
			"coord" : {
				"lat" : 1.0,
				"lon" : 1.0
			},
			"weather" : {
				"temp" : 0
			}
		}
	]
})"_json;

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.Times(1)
		.WillRepeatedly(Return(Clock::from_time_t(110)));
	
	EXPECT_TRUE(m_testObj->Deserialize(json.dump()));

	const auto& values = m_testObj->GetValues();
	ASSERT_EQ(2u, values.size());

	const std::array<double, 2> expectedLatitudes = { 1.1, 1.0 };
	const std::array<double, 2> expectedLongitudes = { 1.1, 1.0 };
	const std::array<nlohmann::json, 2> expectedTemperatures = { "{\"temp\": 1}"_json, "{\"temp\": 0}"_json };
	const std::array<Time, 2> expectedTimes = { Clock::from_time_t(101), Clock::from_time_t(100) };

	auto i = 0u;
	for (const auto& item : values)
	{
		const auto& key = item.first;
		const auto& coords = item.second.first;
		const auto& temperature = item.second.second;
		EXPECT_DOUBLE_EQ(expectedLatitudes[i], coords.GetLatitudeInDegrees());
		EXPECT_DOUBLE_EQ(expectedLongitudes[i], coords.GetLongitudenInDegrees());
		EXPECT_EQ(expectedTemperatures[i], temperature.GetWeatherJson());
		EXPECT_EQ(expectedTimes[i], key);

		++i;
	}
}

TEST_F(TimeCacheTest, DeserializeAndSerialize)
{
	const auto json = R"(
{
	"exp_time" : 30,
	"version" : 1,
	"cache_type" : "time",
	"values" : [
		{
			"time" : 101,
			"coord" : {
				"lat" : 1.1,
				"lon" : 1.1
			},
			"weather" : {
				"temp" : 1
			}
		},
		{
			"time" : 100,
			"coord" : {
				"lat" : 1.0,
				"lon" : 1.0
			},
			"weather" : {
				"temp" : 0
			}
		}
	]
})"_json;

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.Times(1)
		.WillRepeatedly(Return(Clock::from_time_t(110)));

	EXPECT_TRUE(m_testObj->Deserialize(json.dump()));
	EXPECT_EQ(json, nlohmann::json::parse(m_testObj->Serialize()));
}

TEST_F(TimeCacheTest, AllItemsInCache)
{
	const auto json = R"(
{
	"exp_time" : 30,
	"version" : 1,
	"cache_type" : "time",
	"values" : [
		{
			"time" : 104,
			"coord" : {
				"lat" : 2.1,
				"lon" : 3.1
			},
			"weather" : {
				"temp" : -4
			}
		},
		{
			"time" : 101,
			"coord" : {
				"lat" : 1.1,
				"lon" : 1.1
			},
			"weather" : {
				"temp" : 1
			}
		},
		{
			"time" : 100,
			"coord" : {
				"lat" : 1.0,
				"lon" : 1.0
			},
			"weather" : {
				"temp" : 0
			}
		}
	]
})"_json;

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.WillRepeatedly(Return(Clock::from_time_t(150)));

	EXPECT_TRUE(m_testObj->Deserialize(json.dump()));
	ASSERT_EQ(3, m_testObj->GetValues().size());

	const std::array<GeoCoordinates, 3> coordinates = { 
		GeoCoordinates(2.1, 3.1),
		GeoCoordinates(1.1, 1.1),
		GeoCoordinates(1.0, 1.0)
	};
	const std::array<nlohmann::json, 3> temperatures = { 
		"{\"temp\": -4}"_json, 
		"{\"temp\": 1}"_json,
		"{\"temp\": 0}"_json 
	};

	EXPECT_CALL(*m_comparer, AreEqual(coordinates[0], coordinates[0]))
		.WillRepeatedly(Return(true));
	EXPECT_CALL(*m_comparer, AreEqual(coordinates[1], coordinates[1]))
		.WillRepeatedly(Return(true));
	EXPECT_CALL(*m_comparer, AreEqual(coordinates[2], coordinates[2]))
		.WillRepeatedly(Return(true));
	EXPECT_CALL(*m_comparer, AreEqual(coordinates[0], coordinates[1]))
		.WillRepeatedly(Return(false));
	EXPECT_CALL(*m_comparer, AreEqual(coordinates[0], coordinates[2]))
		.WillRepeatedly(Return(false));
	EXPECT_CALL(*m_comparer, AreEqual(coordinates[1], coordinates[0]))
		.WillRepeatedly(Return(false));
	EXPECT_CALL(*m_comparer, AreEqual(coordinates[1], coordinates[2]))
		.WillRepeatedly(Return(false));

	for (auto i = 0u; i < 3; ++i)
	{
		EXPECT_EQ(temperatures[i], m_testObj->GetItem(coordinates[i])->GetWeatherJson());
	}
}

TEST_F(TimeCacheTest, RemoveExpiredItem_WithSameTime)
{
	const auto json = R"(
{
	"exp_time" : 30,
	"version" : 1,
	"cache_type" : "time",
	"values" : [
		{
			"time" : 100,
			"coord" : {
				"lat" : 1.1,
				"lon" : 1.1
			},
			"weather" : {
				"temp" : 1
			}
		},
		{
			"time" : 50,
			"coord" : {
				"lat" : 1.7,
				"lon" : 1.7
			},
			"weather" : {
				"temp" : 1
			}
		},
		{
			"time" : 50,
			"coord" : {
				"lat" : 1.0,
				"lon" : 1.0
			},
			"weather" : {
				"temp" : 0
			}
		}
	]
})"_json;

	EXPECT_CALL(*m_chronoUtils, CurrentTime())
		.Times(1)
		.WillOnce(Return(Clock::from_time_t(50) + m_expirationTime));

	ASSERT_TRUE(m_testObj->Deserialize(json.dump()));

	ASSERT_EQ(1, m_testObj->GetValues().size());
	const auto& item = *m_testObj->GetValues().begin();
	const auto& coords = item.second.first;
	const auto& temperature = item.second.second;
	EXPECT_DOUBLE_EQ(1.1, coords.GetLatitudeInDegrees());
	EXPECT_DOUBLE_EQ(1.1, coords.GetLongitudenInDegrees());
	EXPECT_EQ("{\"temp\": 1}"_json, temperature.GetWeatherJson());
}
