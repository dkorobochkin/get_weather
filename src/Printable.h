#pragma once

class IPrintable
{
public:
	virtual ~IPrintable() = default;

public:
	virtual std::string ToString() const = 0;
};
