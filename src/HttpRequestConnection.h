#pragma once

#include "Connection.h"

class HttpRequestConnection : public Connection
{
public:

	virtual ~HttpRequestConnection() override = default;

public:

	virtual std::string Request(const std::string& url) const override;

};
