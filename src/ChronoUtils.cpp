#include "pch.h"
#include "ChronoUtils.h"

Time ChronoUtils::CurrentTime() const
{
	return Clock::now();
}
