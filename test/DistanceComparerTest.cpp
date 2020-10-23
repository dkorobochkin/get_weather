#include "pch.h"

#include "DistanceComparer.h"
#include "GeoCoordinates.h"
#include "GeoCalculatorMock.h"

using ::testing::Return;

class DistanceComparerTest : public ::testing::Test
{
protected:

	virtual void SetUp() override
	{
		m_calculator = std::make_shared<GeoCalculatorMock>();
		EXPECT_CALL(*m_calculator, GetDistanceError())
			.Times(1)
			.WillOnce(Return(m_distanceError));

		m_testObj = std::make_shared<DistanceComparer>(m_distanceDelta, m_calculator);
	}

protected:

	std::shared_ptr<GeoCalculatorMock> m_calculator;

	std::shared_ptr<DistanceComparer> m_testObj;

	double m_distanceDelta = 10000.0;

	double m_distanceError = 0.005;

};

TEST_F(DistanceComparerTest, AreEquals_SamePoints)
{
	const auto coord = GeoCoordinates(10.0, 20.0);

	EXPECT_CALL(*m_calculator, CalcDistanceInMeters(coord, coord))
		.Times(1)
		.WillOnce(Return(0.0));

	EXPECT_TRUE(m_testObj->AreEqual(coord, coord));
}

TEST_F(DistanceComparerTest, AreEquals_EqualByDistancePoints)
{
	const auto coord1 = GeoCoordinates(10.0, 20.0);
	const auto coord2 = GeoCoordinates(10.0, 20.0);

	EXPECT_CALL(*m_calculator, CalcDistanceInMeters(coord1, coord2))
		.Times(1)
		.WillOnce(Return(m_distanceDelta / 2.0));

	EXPECT_TRUE(m_testObj->AreEqual(coord1, coord2));
}

TEST_F(DistanceComparerTest, AreEquals_NotEqualByDistancePoints)
{
	const auto coord1 = GeoCoordinates(10.0, 20.0);
	const auto coord2 = GeoCoordinates(10.0, 20.0);

	EXPECT_CALL(*m_calculator, CalcDistanceInMeters(coord1, coord2))
		.Times(1)
		.WillOnce(Return(2.0 * m_distanceDelta));

	EXPECT_FALSE(m_testObj->AreEqual(coord1, coord2));
}

TEST_F(DistanceComparerTest, AreEquals_EqualByDistancePointsWithError)
{
	const auto coord1 = GeoCoordinates(10.0, 20.0);
	const auto coord2 = GeoCoordinates(10.0, 20.0);

	EXPECT_CALL(*m_calculator, CalcDistanceInMeters(coord1, coord2))
		.Times(1)
		.WillOnce(Return(m_distanceDelta * (1.0 + m_distanceError / 2.0)));

	EXPECT_TRUE(m_testObj->AreEqual(coord1, coord2));
}
