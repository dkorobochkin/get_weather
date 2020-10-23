#pragma once

#include "Connection.h"

class ConnectionMock : public Connection
{
public:
	MOCK_CONST_METHOD1(Request, std::string(const std::string&));
};
