#pragma once

#include "Printable.h"

class Weather : public IPrintable
{
public:

	Weather(const nlohmann::json& weatherJson);

	virtual ~Weather() = default;

public:

	const nlohmann::json& GetWeatherJson() const;

	virtual std::string ToString() const override;

	bool operator==(const Weather& other) const;

	bool operator!=(const Weather& other) const;

private:

	const nlohmann::json m_weatherJson;

};