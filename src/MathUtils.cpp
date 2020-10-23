#include "pch.h"
#include "MathUtils.h"

bool MathUtils::Compare(double d1, double d2)
{
	return std::fabs(d1 - d2) <= std::numeric_limits<double>::epsilon();
}
