#pragma once

#include "ChronoUtils.h"

class ChronoUtilsMock : public IChronoUtils
{
public:
	MOCK_CONST_METHOD0(CurrentTime, Time());
};
