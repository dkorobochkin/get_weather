#pragma once

class IChronoUtils
{
public:

	virtual ~IChronoUtils() = default;

public:

	virtual Time CurrentTime() const = 0;

};

class ChronoUtils : public IChronoUtils
{
public:

	virtual ~ChronoUtils() override = default;

public:

	virtual Time CurrentTime() const override;

};
