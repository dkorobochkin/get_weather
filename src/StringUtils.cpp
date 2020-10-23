#include "pch.h"
#include "StringUtils.h"

std::string StringUtils::DoubleToString(double d, std::streamsize precision)
{
	std::ostringstream stream;

	stream << std::fixed;
	stream << std::setprecision(precision);
	stream << d;

	return stream.str();
}
