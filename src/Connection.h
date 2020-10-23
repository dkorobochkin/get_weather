#pragma once

class Connection
{
public:

	virtual ~Connection() = default;

public:

	virtual std::string Request(const std::string& url) const = 0;

};
