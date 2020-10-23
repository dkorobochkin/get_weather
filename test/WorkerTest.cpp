#include "pch.h"

#include "Worker.h"
#include "CacheMock.h"
#include "WeatherGetterMock.h"

using ::testing::Return;

class WorkerTest : public ::testing::Test
{
protected:

	virtual void SetUp() override
	{
		m_cache = std::make_shared<CacheMock>();
		m_weatherGetter = std::make_shared<WeatherGetterMock>();
		m_testObj = std::make_shared<Worker>(m_cache, m_weatherGetter, m_stopEvent);

		m_coords = std::make_shared<GeoCoordinates>(10.0, 20.0);
		m_weather = std::make_shared<Weather>("{\"temp\": -40.3}"_json);
	}

protected:

	std::shared_ptr<CacheMock> m_cache;

	std::shared_ptr<WeatherGetterMock> m_weatherGetter;

	std::shared_ptr<Worker> m_testObj;

	std::atomic_bool m_stopEvent = false;

	std::shared_ptr<GeoCoordinates> m_coords;

	std::shared_ptr<Weather> m_weather;

};

TEST_F(WorkerTest, Run_StopEventOnStart)
{
	EXPECT_CALL(*m_cache, GetItem(*m_coords))
		.Times(0);
	EXPECT_CALL(*m_cache, AddItem(*m_coords, *m_weather))
		.Times(0);

	EXPECT_CALL(*m_weatherGetter, Get(*m_coords))
		.Times(0);

	m_stopEvent = true;
	m_testObj->Run(*m_coords);
}

TEST_F(WorkerTest, Run_ItemInCache)
{
	EXPECT_CALL(*m_cache, GetItem(*m_coords))
		.Times(1)
		.WillOnce(Return(*m_weather));
	EXPECT_CALL(*m_cache, AddItem(*m_coords, *m_weather))
		.Times(0);

	EXPECT_CALL(*m_weatherGetter, Get(*m_coords))
		.Times(0);

	m_testObj->Run(*m_coords);
}

TEST_F(WorkerTest, Run_ItemNotInCache)
{
	EXPECT_CALL(*m_cache, GetItem(*m_coords))
		.Times(1)
		.WillOnce(Return(std::nullopt));
	EXPECT_CALL(*m_cache, AddItem(*m_coords, *m_weather))
		.Times(1);

	EXPECT_CALL(*m_weatherGetter, Get(*m_coords))
		.Times(1)
		.WillOnce(Return(*m_weather));

	m_testObj->Run(*m_coords);
}