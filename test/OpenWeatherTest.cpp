#include "pch.h"

#include "GeoCoordinates.h"
#include "ConnectionMock.h"
#include "OpenWeather.h"
#include "Weather.h"

using ::testing::Return;

class OpenWeatherTestable : public OpenWeather
{
public:
	using OpenWeather::OpenWeather;

	std::string GetUrl(const GeoCoordinates& coordinates) const
	{
		return PrepareUrl(coordinates);
	}
};

class OpenWeatherTest : public ::testing::Test
{
protected:

	virtual void SetUp() override
	{
		m_connection = std::make_shared<ConnectionMock>();
		m_testObj = std::make_shared<OpenWeatherTestable>(m_connection, m_apiId);
		
		m_coords = std::make_shared<GeoCoordinates>(10.0, 20.0);
		m_response = R"({
				"base": "stations",
				"clouds": {
					"all": 100
				},
				"cod": 200,
				"coord": {
					"lat": 10.0,
					"lon": 20.0
				},
				"main": {
					"temp": -13.02,
					"temp_max": -13.02,
					"temp_min": -13.02
				}
			})"_json;
	}

protected:

	std::shared_ptr<ConnectionMock> m_connection;

	std::shared_ptr<OpenWeatherTestable> m_testObj;

	std::shared_ptr<GeoCoordinates> m_coords;

	nlohmann::json m_response;

	const std::string m_apiId = "12345";

	const std::string m_url = "api.openweathermap.org/data/2.5/weather?lat=10.000000&lon=20.000000&appid=12345&units=metric";

};

TEST_F(OpenWeatherTest, PrepareUrl)
{
	EXPECT_EQ(m_url, m_testObj->GetUrl(*m_coords));
}

TEST_F(OpenWeatherTest, Get_Success)
{
	EXPECT_CALL(*m_connection, Request(m_url))
		.Times(1)
		.WillOnce(Return(m_response.dump()));

	EXPECT_EQ(Weather(m_response), m_testObj->Get(*m_coords));
}

TEST_F(OpenWeatherTest, Get_IncorrectApiKey)
{
	m_response["cod"] = 401;

	EXPECT_CALL(*m_connection, Request(m_url))
		.Times(1)
		.WillOnce(Return(m_response.dump()));

	ASSERT_THROW(m_testObj->Get(*m_coords), std::exception);
}

TEST_F(OpenWeatherTest, Get_IncorrectRequest)
{
	m_response["cod"] = 404;

	EXPECT_CALL(*m_connection, Request(m_url))
		.Times(1)
		.WillOnce(Return(m_response.dump()));

	ASSERT_THROW(m_testObj->Get(*m_coords), std::exception);
}

TEST_F(OpenWeatherTest, Get_ManyRequests)
{
	m_response["cod"] = 429;

	EXPECT_CALL(*m_connection, Request(m_url))
		.Times(1)
		.WillOnce(Return(m_response.dump()));

	ASSERT_THROW(m_testObj->Get(*m_coords), std::exception);
}
